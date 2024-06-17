#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <errno.h>

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

    // ファイルが存在し、かつRead権限があることをチェック
    errno = 0;
    if(access(argv[1], R_OK) != 0){
        std::cerr << "Error (access): " << strerror(errno) << std::endl;
        return 1;
    }

    errno = 0;
    const int fd = open(argv[1], O_RDONLY);

    if(fd == -1){
        std::cerr << "Error (open): " << strerror(errno) << std::endl;
        return 1;
    }

    errno = 0;
    char * in = (char*)mmap(nullptr, fsize, PROT_READ, MAP_SHARED, fd, 0);
    if(in == MAP_FAILED){
        std::cerr << "Error (mmap): " << strerror(errno) << std::endl;
        return 1;
    }

    memcpy(data.data(), in, fsize);

    errno = 0;
    if(munmap(in, fsize) == -1){          // これを呼ばないとリークする
        std::cerr << "Error (munmap): " << strerror(errno) << std::endl;
        return 1;
    }

    errno = 0;
    if(close(fd) == -1){
        std::cerr << "Error (close): " << strerror(errno) << std::endl;
        return 1;
    }

    sw.stop();
    std::cerr << "reading file has taken " << sw.getResult() * 1000 <<  " ms" << std::endl;

    std::cout << data.data() << '\0' << std::endl;

    return 0;
}


