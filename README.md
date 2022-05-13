# C++ SHA256 Implementation.

### Usage
```C++
#include <iostream>
#include "sha256.h"

int main(){
  std::string data = "Data";
  woXrooX::Sha256 hash;
  std::cout << hash.digest(data) << '\n';
  
  return 0;
}
```

### Output
```
cec3a9b89b2e391393d0f68e4bc12a9fa6cf358b3cdf79496dc442d52b8dd528
```
