#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include "twiddle.h"
#include <math.h>

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main()
{
  uWS::Hub h;
  bool tune_params = false;
  int iter = 0;
  int iter_start = 0;
  int iter_total = 5100;
  double error = 0.0;
  double tol = 0.1;
  std::vector<double> params;

  if(tune_params) {
    params.push_back(0.210256);
    params.push_back(0.00012111);
    params.push_back(4.019);
    std::cout << std::endl;
    std::cout << "####### Tuning parameters #######" << std::endl;
    std::cout << std::endl;
    std::cout << "param values: "
              << "Kp = " << params[0]
              << " Ki = " << params[1]
              << " Kd = " << params[2] << std::endl;
}
  else {
    params.push_back(0.220256);
    params.push_back(0.000119051);
    params.push_back(3.93977);
  }

  Twiddle t;
  t.init(params);

  PID pid;
  pid.Init(params[0], params[1], params[2]);

  h.onMessage([&t, &pid, tol, error, iter, iter_total, iter_start, tune_params, params](uWS::WebSocket<uWS::SERVER> ws, 
              char *data, size_t length, uWS::OpCode opCode) mutable {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          double angle = std::stod(j[1]["steering_angle"].get<std::string>());
          /*
          * TODO: Calcuate steering value here, remember the steering value is
          * [-1, 1].
          * NOTE: Feel free to play around with the throttle and speed. Maybe use
          * another PID controller to control the speed!
          */

          if(tune_params) {
            iter++;

            // calculate squared cte
            if(iter >= iter_start) {
              error += cte * cte;
            }
            
            if (iter >= iter_total) {
              std::cout << "error = " << error << std::endl;
              // tune parameters
              t.tuneParams(error);
              // get updated parameters
              t.getParams(params);
              std::cout << "param values: "
                        << "Kp = " << params[0]
                        << " Ki = " << params[1]
                        << " Kd = " << params[2] << std::endl;
              // initialize pid controller with updated parameters
              pid.Init(params[0], params[1], params[2]);

              double sum_d_params = t.getSumdParams();
              std::cout << "sum of d_params = " << sum_d_params << std::endl;
              // check if optimal parameters have been found
              if(sum_d_params < tol && error < 1000) {
                std::cout << "Optimal param values: "
                          << "Kp = " << params[0]
                          << " Ki = " << params[1]
                          << " Kd = " << params[2] << std::endl;
                
                // turn off parameter tuning
                tune_params = false;
              }
              
              // reset simulator
              iter = 0;
              error = 0.0;
              std::string msg = "42[\"reset\",{}]";
              ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
            }
          }

          pid.UpdateError(cte);
          double steer_value = pid.TotalError();
          
          // DEBUG
          //std::cout << "CTE: " << cte << " Steering Value: " << steer_value << std::endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = 0.3;
          //msgJson["throttle"] = 0.5;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          //std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}

