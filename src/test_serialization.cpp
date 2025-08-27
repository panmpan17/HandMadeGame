#include <linmath.h>
#include <fstream> 
#include <iostream>
#include <vector>
#include "node.h"
#include "serializer.h"



// int main()
// {
//   Node myNode(3.0f, 2.0f, 3.0f, 45.0f);

//   std::cout << myNode << std::endl;

//   try
//   {
//     DataSerializer serializer("output.txt");
//     myNode.serializedTo(serializer);
//   }
//   catch(const std::exception& e)
//   {
//     std::cerr << e.what() << '\n';
//   }

//   DataDeserializer deserializer("output.txt");
//   deserializer.read();

//   return 0;
// }

// g++ src/test_serialization.cpp src/node.cpp src/serializer.cpp -I./include -std=c++23 ; ./a.out