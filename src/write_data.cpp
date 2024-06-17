#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
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
    // - 最初はWrite権限のみにしておく. Readはあとでつける（Write中に他のプロセスがReadしないなら今つけてもいい）
    // - O_EXCL は、このopenでファイルの新規作成にならない場合にエラーにする。
    errno = 0;
    const int fd = open(argv[1], O_RDWR | O_CREAT | O_EXCL, S_IWUSR);
    // const int fd = open(argv[1], O_RDWR | O_CREAT,
    //                     S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

    if(fd == -1){
        std::cerr << "Error (open): " << strerror(errno) << std::endl;
        return 1;
    }

    errno = 0;
    if(ftruncate(fd, fsize) == -1){
        std::cerr << "Error (ftruncate): " << strerror(errno) << std::endl;
        return 1;
    }

    errno = 0;
    char * out = (char*)mmap(nullptr, fsize, PROT_WRITE, MAP_SHARED, fd, 0);
    if(out == MAP_FAILED){
        std::cerr << "Error (mmap): " << strerror(errno) << std::endl;
        return 1;
    }

    // std::string buf;
    // std::getline(std::cin, buf);

    std::memcpy(out, &(data[0]), fsize);

    errno = 0;
    if(close(fd) == -1){
        std::cerr << "Error (close): " << strerror(errno) << std::endl;
        return 1;
    }

    errno = 0;
    if(munmap(out, fsize) == -1){          // これを呼ばないとリークする
        std::cerr << "Error (munmap): " << strerror(errno) << std::endl;
        return 1;
    }

    // Writeが終わってからRead可能にする
    // これで「まだWriteが終わっていないのにReadされる」事態は防げる。
    errno = 0;
    if(chmod(argv[1], S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH) == -1){
        std::cerr << "Error (chmod): " << strerror(errno) << std::endl;
        return 1;
    }
    
    sw.stop();
    std::cout << "writing file has taken " << sw.getResult() * 1000 <<  " ms" << std::endl;
    
    
    return 0;
}


