g++ -o CLI2 CLI2.cpp -lboost_program_options
#Before adding sender.cpp
g++ -o CLI3 CLI3.cpp -lofstd -lboost_program_options
#After adding sender.cpp
g++ -o CLI3 CLI3.cpp communication/sender.cpp -ldcmnet -lofstd -lboost_program_options
#TODO: Implement both receiver and sender.cpp