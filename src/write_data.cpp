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

    std::vector<char> data(fsize, 0);
    for(int i = 0; i < fsize; i++){
        data[i] = 'A' + (char)(i % 26);
    }

    my_utils_kk4::StopWatch sw;
    sw.start();

    // - O_WRONLYだとmemcpyでSegmentation faultになる
    // - ファイルの新規作成を許容するためにO_CREATが必要
    // - O_CREAT指定時は、作成するファイルのパーミッションを第3引数に指定が必要
    const int fd = open(argv[1], O_RDWR | O_CREAT,
                        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

    if(fd == -1){
        std::cerr << "file open error" << std::endl;
        return 1;
    }
    
    if(ftruncate(fd, fsize) == -1){
        std::cerr << "ftruncate fail" << std::endl;
        return 1;
    }

    

    char * out = (char*)mmap(nullptr, fsize, PROT_WRITE, MAP_SHARED, fd, 0);

    std::memcpy(out, &(data[0]), fsize);

    close(fd);

    sw.stop();
    std::cout << "writing file has taken " << sw.getResult() * 1000 <<  " ms" << std::endl;
    
    
    return 0;
}


