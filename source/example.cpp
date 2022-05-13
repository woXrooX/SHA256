#include <iostream>
#include "sha256.h"

int main(){
  std::string data = "Data";
  woXrooX::MET met;
  woXrooX::Sha256 hash;
  std::cout << hash.digest(data) << '\n';

  return 0;
}
