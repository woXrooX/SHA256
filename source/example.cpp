#include <iostream>
#include "sha256.h"

int main(int argc, char const *argv[]){

  woXrooX::Sha256 hash("data");
  std::cout << hash.digest() << '\n';

  return 0;
}
