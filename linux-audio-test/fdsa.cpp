#include <iostream>
#include <vector>
#include <cstdio>
#include <iterator>
#include <cstdint>
#include <fstream>

int main()
{
	std::cout << "started:" << std::endl;
	int nSignedInts = 3;
	int bitsInFormat = 16;
	int bytesCount = nSignedInts * (bitsInFormat / 8); // how about  sizeof(int16_t) instead of bitsInFormat / 8  ?
	std::vector<int16_t> buffer(nSignedInts);

	//fread(buffer.data(), sizeof(int16_t), nSignedInts, std::cin);
	std::cin.read((char*)buffer.data(), bytesCount);

	for(int i = 0; i < buffer.size(); i++)
	{
		std::cout << buffer[i] << std::endl;	
	}
	std::vector<int16_t> out;
	out.push_back(10);
	out.push_back(20);
	out.push_back(30);
	out.push_back(40);
	std::ofstream outfile ("new.txt",std::ofstream::binary);
	outfile.write((char*)out.data(), bytesCount);
}
