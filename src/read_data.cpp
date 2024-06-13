#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>

#include <iostream>
#include <vector>

#include <my_utils_kk4.hpp>

int main(int argc, char** argv){

    if(argc != 3){
        std::cerr << "Usage: " << argv[0] << " <filename> <filesize in byte>"<< std::endl;
        return 1;
    }
    const int fsize = std::stoi(argv[2]);
    std::vector<char> data(fsize);
    
    my_utils_kk4::StopWatch sw;
    sw.start();

    const int fd = open(argv[1], O_RDONLY);

    if(fd == -1){
        std::cerr << "file open error" << std::endl;
        return 1;
    }

    

    char * in = (char*)mmap(nullptr, fsize, PROT_READ, MAP_SHARED, fd, 0);

    memcpy(data.data(), in, fsize);

    close(fd);

    sw.stop();
    std::cerr << "reading file has taken " << sw.getResult() * 1000 <<  " ms" << std::endl;

    std::cout << data.data() << '\0' << std::endl;

    return 0;
}


