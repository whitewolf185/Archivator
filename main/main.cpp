#include "Archivator.h"
#include <queue>
#include <time.h>

namespace myExploration{
    uint64_t Get_fileSize(const std::string& path){
        uint64_t result;

        // ----opening file----
        std::ifstream fin;
        fin.open(path, std::ios::binary);

        if(!fin.is_open())
            throw std::runtime_error("Cannot open file " + path);
        //--------

        fin.seekg(0,std::ios::end);
        result = fin.tellg();
        fin.close();

        return result;
    }

    void Write_file_in_mainFile(std::ofstream& fout, const std::string& path){
        char c;
        // ----opening file----
        std::ifstream fin;
        fin.open(path, std::ios::binary);

        if(!fin.is_open())
            throw std::runtime_error("Cannot open file " + path);
        //--------

        while(fin.read((char *) &(c), sizeof (char))){
            fout.write((const char *) &(c), sizeof (char));
        }

        fin.close();
    }

    void Copy_file_to_mainFile(std::ofstream& fout, std::ifstream& fin){
        char c;
        while(fin.read((char *) &(c), sizeof (char))){
            fout.write((const char *) &(c), sizeof (char));
        }
    }

    struct metaData{
        uint64_t size = 0;
        uint64_t decoding_size = 0;
        uint32_t name_size = 0;
        std::string name;
    };

    void Parse_to_files(const std::string& path){

        //----opening file----
        std::ifstream fin;
        fin.open(path, std::ios::binary);

        if(!fin.is_open())
            throw std::runtime_error("Cannot open file " + path);
        //---------


        int size_of_fileName;
        while(fin.read((char *) &(size_of_fileName), sizeof (int))){
            std::string filename;

            for (int i = 0; i < size_of_fileName; ++i) {
                char c;
                fin.read((char *) &(c), sizeof (char));
                filename += c;
            }

            uint64_t size_of_file;
            fin.read((char *) &(size_of_file), sizeof (size_of_file));

            std::ofstream fout;
            fout.open(filename, std::ios::binary);

            if(!fout.is_open())
                throw std::runtime_error("Cannot open file " + filename);

            for (uint64_t i = 0; i < size_of_file; ++i) {
                char c;
                fin.read((char *) &(c), sizeof (char));
                fout.write((const char *) &(c),sizeof (char));
            }
            fout.close();

        }

        fin.close();



    }
}

int main(int argc, char* argv[]){

    std::queue<std::string> files;

    Settings settings;

    //key parsing
    for (int i = 1; i < argc; ++i) {
        if(argv[i] == (std::string)"-c"){
            settings.write_output_into_stdout = true;
        }

        else if(argv[i] == (std::string)"-k"){
            settings.donot_delete = true;
        }

        else if(argv[i] == (std::string)"-l"){
            settings.show_metaData = true;
        }

        else if(argv[i] == (std::string)"-r"){
            settings.recursive = true;
        }

        else if(argv[i] == (std::string)"-t"){
            settings.test_unityFile = true;
        }

        else if(argv[i] == (std::string)"-1"){
            settings.sizeBit = Settings::Block_sizes::bit16;
            settings.Block_chooser();
        }

        else if(argv[i] == (std::string)"-2"){
            settings.sizeBit = Settings::Block_sizes::bit24;
            settings.Block_chooser();
        }

        else if(argv[i] == (std::string)"-3"){
            settings.sizeBit = Settings::Block_sizes::bit32;
            settings.Block_chooser();
        }

        else if(argv[i] == (std::string)"-4"){
            settings.sizeBit = Settings::Block_sizes::my;
            settings.Block_chooser();
        }

        else if(argv[i] == (std::string)"-d"){
            settings.decompress = true;
        }

        else{
            files.push(argv[i]);
        }
    }

    if(files.empty()){
        std::cerr << "you do not enter any files" << std::endl;
        return 1;
    }

    if(settings.show_metaData){
        try {
            myExploration::metaData data;

            std::ifstream fin;
            auto file = files.front();
            while(!files.empty()){
                files.pop();
            }
            fin.open(file, std::ios::binary);

            if(!fin.is_open())
                throw std::runtime_error("Cannot open file " + file);

            fin.read((char *) &(data.size), sizeof(data.size));
            fin.read((char *) &(data.decoding_size), sizeof(data.decoding_size));
            fin.read((char *) &(data.name_size), sizeof(data.name_size));
            for (int i = 0; i < data.name_size; ++i) {
                char c;
                fin.read((char *) &(c), sizeof(char));
                data.name += c;
            }

            std::cout << data.size << " byte" << std::endl;
            std::cout << data.decoding_size << " byte" << std::endl;
            std::cout << 100 - (double)data.size / (double)data.decoding_size * 100 << "%" << std::endl;
            std::cout << "multyname: " << data.name << std::endl;

        }
        catch (std::runtime_error &error) {
            error.what();
        }

        char c;
        std::cout << "do you want to proceed? y or n" << std::endl;
        std::cin >> c;
        if(c == 'n'){
            return 0;
        }

        else if(c != 'y'){
            std::cerr << "I cannot understand you...";
            return 1;
        }

    }

    //compressing
    if(!settings.decompress){
        try {
            if (files.size() <= 1){
                std::cerr << "Please enter 2 or more files" << std::endl;
                return 1;
            }
            else{
                mkdir("tmp");
                std::ofstream fout;
                fout.open("./tmp/unifited_file.tmp", std::ios::binary);

                myExploration::metaData data;

                double start = clock();

                while(files.size() > 1) {
                    std::string file = files.front();
                    files.pop();

                    data.name_size += file.size() + 1;
                    data.name += file + ' ';

                    /*
                     * сначала записывается число, равное количеству символов в имени файлов
                     * потом записывается само имя
                     * потом записывается long long сколько байт в данном подфайле
                     * потом само содержимое файла
                     */

                    int size_of_fileName = file.size();

                    //размер
                    fout.write((const char *) &(size_of_fileName), sizeof(int));
                    //само имя
                    for (int i = 0; i < size_of_fileName; ++i) {
                        fout.write((const char *) &(file[i]), sizeof(char));
                    }
                    //размер файла
                    uint64_t file_size = myExploration::Get_fileSize(file);
                    fout.write((const char *) &(file_size), sizeof(file_size));
                    //сам файл
                    myExploration::Write_file_in_mainFile(fout, file);

                    if(!settings.donot_delete){
                        std::remove(file.c_str());
                    }
                }
                fout.close();

                data.decoding_size = myExploration::Get_fileSize("./tmp/unifited_file.tmp");


                Archivator arc("./tmp/unifited_file.tmp", settings);
                auto output_file = files.front();
                files.pop();
                arc.Compress("./tmp/result.tmp");
                data.size = myExploration::Get_fileSize("./tmp/result.tmp");

                if (settings.donot_delete){
                    std::remove("./tmp/unifited_file.tmp");
                }

                /*Сначала идет заархивированный размер
                 * Потом идет разархивированный размер
                 * потом счетчик имени
                 * потом сами имена*/
                fout.open(output_file, std::ios::binary);
                if(!fout.is_open()){
                    throw std::runtime_error("Cannot open file " + output_file);
                }

                fout.write((const char *) &(data.size), sizeof (data.size));
                fout.write((const char *) &(data.decoding_size), sizeof (data.decoding_size));
                fout.write((const char *) &(data.name_size), sizeof (data.name_size));

                fout.write((const char *) (data.name.data()), sizeof (char) * data.name.size());

                myExploration::Write_file_in_mainFile(fout,"./tmp/result.tmp");
                std::remove("./tmp/result.tmp");
                fout.close();

                rmdir("tmp");

                double end = clock();

                std::cout << (end - start)/CLOCKS_PER_SEC << " sec" << std::endl;
            }
        }
        catch (std::runtime_error& error){
            error.what();
        }
    }

    else{
        myExploration::metaData data;

        std::ifstream fin;
        auto file = files.front();
        while(!files.empty()){
            files.pop();
        }
        fin.open(file, std::ios::binary);

        if(!fin.is_open())
            throw std::runtime_error("Cannot open file " + file);

        fin.read((char *) &(data.size), sizeof(data.size));
        fin.read((char *) &(data.decoding_size), sizeof(data.decoding_size));
        fin.read((char *) &(data.name_size), sizeof(data.name_size));
        for (int i = 0; i < data.name_size; ++i) {
            char c;
            fin.read((char *) &(c), sizeof(char));
            data.name += c;
        }

        mkdir("tmp");

        std::ofstream fout;
        fout.open("./tmp/decoding.tmp", std::ios::binary);
        if(!fout.is_open()){
            throw std::runtime_error("Cannot open file decoding.tmp");
        }

        myExploration::Copy_file_to_mainFile(fout,fin);
        fout.close();
        fin.close();

        Archivator arc(settings);
        arc.Decompress("./tmp/decoding.tmp", "./tmp/result.tmp");

        myExploration::Parse_to_files("./tmp/result.tmp");

        if(!settings.donot_delete){
            std::remove(file.c_str());
        }
        else{
            std::remove("./tmp/decoding.tmp");
        }
        std::remove("./tmp/result.tmp");
        rmdir("tmp");
    }
        return 0;
}