#ifndef SHA256_H
#define SHA256_H

#include <string>
#include <bit> // For std::rotr
#include <bitset> // For std::bitset
#include <iomanip> // For stringstream

namespace woXrooX{
  class Sha256{
  public:
    Sha256(const std::string &data) : data(data){
      this->hash();
    }
    ~Sha256(){}

    // Returns Hashed Data AKA Result
    std::string digest(){
      return this->result;
    }
  private:
    void hash(){
      std::string data_in_binary;
      int zero_bits_size = zero_bits(this->data.length()*8);

      //////////////// Converting Data To Binary
      for(int i = 0; i < this->data.length(); i++){
        data_in_binary.append(std::bitset<8>(this->data[i]).to_string());
      }

      //////////////// Appending One Bit To The Data Before Filling With 0's Reamianing Bits
      data_in_binary.append("1");

      //////////////// Filling With 0's
      for(int i = 0; i < zero_bits_size; i++){
        data_in_binary.append("0");
      }

      //////////////// Appending 64 Bits, Representing The Length Of Original Input
      data_in_binary.append(std::bitset<64>(this->data.length()*8).to_string());

      //////////////// Break data_in_binary Into 512-bit Chunks
      unsigned int data_512_bit_chunks_size = data_in_binary.length()/512;
      std::string data_512_bit_chunks[data_512_bit_chunks_size];
      for(int i = 0; i < data_512_bit_chunks_size; i++){
        data_512_bit_chunks[i] = data_in_binary.substr(512*i, 512);
      }

      //////////////// Creating A 64-Entry Message Schedule Array w[0..63] Of 32-Bit Words. Spliting Each Block Into 32-bit Words (512 / 32 = 16)
      std::string data_32_bit_words[data_512_bit_chunks_size][64];
      for(int i = 0; i < data_512_bit_chunks_size; i++){
        for(int j = 0; j < 16; j++){
          data_32_bit_words[i][j] = data_512_bit_chunks[i].substr(32*j, 32);
        }
      }

      //////////////// Extend The First 16 Words Into The Remaining 48 Words w[16..63] Of The Message Schedule Array
      // data_32_bit_words[i][j] = ls1(j-2)+(j-7)+ls0(j-15)+(j-16)
      for(int i = 0; i < data_512_bit_chunks_size; i++){
        for(int j = 16; j < 64; j++){
          data_32_bit_words[i][j] =
            std::bitset<32>(
              ls1(std::bitset<32>(data_32_bit_words[i][j-2])).to_ulong()+
              std::bitset<32>(data_32_bit_words[i][j-7]).to_ulong()+
              ls0(std::bitset<32>(data_32_bit_words[i][j-15])).to_ulong()+
              std::bitset<32>(data_32_bit_words[i][j-16]).to_ulong()
            ).to_string()
          ;
        }
      }

      //////////////// Initialize working variables to current hash value (Compression)
      // tmpW1 = us1(e) + choice(e, f, g) + h + k[0] + data_32_bit_words[0][0];
      // tmpW2 = us0(a) + majority(a, b, c);
      uint32_t h0 = this->h0;
      uint32_t h1 = this->h1;
      uint32_t h2 = this->h2;
      uint32_t h3 = this->h3;
      uint32_t h4 = this->h4;
      uint32_t h5 = this->h5;
      uint32_t h6 = this->h6;
      uint32_t h7 = this->h7;

      uint32_t a = this->h0;
      uint32_t b = this->h1;
      uint32_t c = this->h2;
      uint32_t d = this->h3;
      uint32_t e = this->h4;
      uint32_t f = this->h5;
      uint32_t g = this->h6;
      uint32_t h = this->h7;

      for(int i = 0; i < data_512_bit_chunks_size; i++){
        for(int j = 0; j < 64; j++){
          std::string tmp_w1 =
            std::bitset<32>(
              us1(e).to_ulong()+
              choice(std::bitset<32>(e), std::bitset<32>(f), std::bitset<32>(g)).to_ulong()+
              h+
              this->K[j]+
              std::bitset<32>(data_32_bit_words[i][j]).to_ulong()
            ).to_string()
          ;

          std::string tmp_w2 =
            std::bitset<32>(
              us0(std::bitset<32>(a)).to_ulong()+
              majority(std::bitset<32>(a), std::bitset<32>(b), std::bitset<32>(c)).to_ulong()
            ).to_string()
          ;

          h = g;
          g = f;
          f = e;
          e = d + std::bitset<32>(tmp_w1).to_ulong();
          d = c;
          c = b;
          b = a;
          a = std::bitset<32>(tmp_w1).to_ulong() + std::bitset<32>(tmp_w2).to_ulong();
        }
        h0 = h0 + a;
        h1 = h1 + b;
        h2 = h2 + c;
        h3 = h3 + d;
        h4 = h4 + e;
        h5 = h5 + f;
        h6 = h6 + g;
        h7 = h7 + h;

        a = h0;
        b = h1;
        c = h2;
        d = h3;
        e = h4;
        f = h5;
        g = h6;
        h = h7;
      }

      // To Hex String
      std::stringstream ss;
      ss << std::hex << h0 << h1 << h2 << h3 << h4 << h5 << h6 << h7;
      std::string result(ss.str());
      this->result = result;
    }

    // Calculates How Many Bits Needs To Be Filled With 0's
    int zero_bits(int S){
      int m = 1;
      int zero_bits_size = 0;
      if(S+65 > 512){
        m = ( (S + 65) / 512.0)+1;
      }
      zero_bits_size = (512*m)-65-S;
      return zero_bits_size;
    }

    ///////////////// Round Functions START
    ///// choice
    // result = (e and f) xor ((not e) and g)
    std::bitset<32> choice(std::bitset<32> e, std::bitset<32> f, std::bitset<32> g){
      return (e & f) ^ ((~e) & g);
    }

    ///// Majority
    // result = (a and b) xor (a and c) xor (b and c)
    std::bitset<32> majority(std::bitset<32> a, std::bitset<32> b, std::bitset<32> c){
      return (a & b) ^ (a & c) ^ (b & c);
    }

    ///// Lower Case Sigma 0
    // result = rotr(b, 7) ^ rotr(b, 18) ^ (b >> 3)
    std::bitset<32> ls0(std::bitset<32> b){
      uint32_t tmp_b = (uint32_t)b.to_ulong();

      std::bitset<32> rr7 = std::bitset<32>(std::rotr(tmp_b, 7));
      std::bitset<32> rr18 = std::bitset<32>(std::rotr(tmp_b, 18));
      std::bitset<32> shr3 = b >> 3;
      return rr7^rr18^shr3;
    }

    ///// Lower Case Sigma 1
    // result = rotr(b, 17) ^ rotr(b, 19) ^ (b >> 10)
    std::bitset<32> ls1(std::bitset<32> b){
      uint32_t tmp_b = (uint32_t)b.to_ulong();

      std::bitset<32> rr17 = std::bitset<32>(std::rotr(tmp_b, 17));
      std::bitset<32> rr19 = std::bitset<32>(std::rotr(tmp_b, 19));
      std::bitset<32> shr10 = b >> 10;
      return rr17^rr19^shr10;
    }

    ///// Upper Case Sigma 0
    // result = rotr(b, 2) ^ rotr(b, 13) ^ rotr(b, 22)
    std::bitset<32> us0(std::bitset<32> b){
      uint32_t tmp_b = (uint32_t)b.to_ulong();

      std::bitset<32> rr2 = std::bitset<32>(std::rotr(tmp_b, 2));
      std::bitset<32> rr13 = std::bitset<32>(std::rotr(tmp_b, 13));
      std::bitset<32> rr22 = std::bitset<32>(std::rotr(tmp_b, 22));
      return rr2^rr13^rr22;
    }

    ///// Upper Case Sigma 1
    // result = rotr(b, 6) ^ rotr(b, 11) ^ rotr(b, 25)
    std::bitset<32> us1(std::bitset<32> b){
      uint32_t tmp_b = (uint32_t)b.to_ulong();

      std::bitset<32> rr6 = std::bitset<32>(std::rotr(tmp_b, 6));
      std::bitset<32> rr11 = std::bitset<32>(std::rotr(tmp_b, 11));
      std::bitset<32> rr25 = std::bitset<32>(std::rotr(tmp_b, 25));
      return rr6^rr11^rr25;
    }
    ///////////////// Round Functions END

    std::string data;
    std::string result;


    // Initialize hash values: (first 32 bits of the fractional parts of the square roots of the first 8 primes 2..19)
    const uint32_t h0 = 0x6a09e667;
    const uint32_t h1 = 0xbb67ae85;
    const uint32_t h2 = 0x3c6ef372;
    const uint32_t h3 = 0xa54ff53a;
    const uint32_t h4 = 0x510e527f;
    const uint32_t h5 = 0x9b05688c;
    const uint32_t h6 = 0x1f83d9ab;
    const uint32_t h7 = 0x5be0cd19;

    // Initialize array of round constants: (first 32 bits of the fractional parts of the cube roots of the first 64 primes 2..311)
    const uint32_t K[64] = {
      0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
      0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
      0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
      0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
      0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
      0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
      0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
      0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };
  };
}

#endif
