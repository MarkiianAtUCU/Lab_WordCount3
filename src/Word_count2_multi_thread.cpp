#include <iostream>
#include "boost/filesystem.hpp"
#include "../dependencies/FileProcessor.h"
#include <vector>
#include "boost/locale.hpp"
#include "../dependencies/time_meter.h"
#include "../dependencies/config_reader.h"
#include <thread>
#include <mutex>
#include <boost/iterator/distance.hpp>
#include "../dependencies/thread_safe_queue.h"
#include <chrono>
#include "QCoreApplication"
#include "QtConcurrentMap"
#include "QStringList"


std::map<std::string, int> process_text(const std::string & elem) {
    std::string elem_n =  boost::locale::normalize(elem);
    boost::locale::boundary::ssegment_index map(boost::locale::boundary::word,elem_n.begin(),elem_n.end());
    map.rule(boost::locale::boundary::word_letters);
    elem_n.clear();
    std::map<std::string, int> thread_res_map;

    for(boost::locale::boundary::ssegment_index::iterator it=map.begin(),e=map.end(); it!=e; ++it) {
        thread_res_map[boost::locale::fold_case(it -> str())]++;
    }
    return thread_res_map;
}

std::map<std::string, int> do_nothing (const std::map<std::string, int> & elem) {
    return elem;
};

#include <unistd.h>
#define GetCurrentDir getcwd



int main(int argc, char **argv) {
    auto start = get_current_time_fenced();
    boost::locale::generator gen;
    std::locale loc = gen("");
    std::locale::global(loc);
    std::wcout.imbue(loc);
    std::ios_base::sync_with_stdio(false);



    auto config_object = config("../config.txt");
    std::string in_file = config_object.get_string("in_file");
    in_file.erase(std::remove(in_file.begin(), in_file.end(), '\r'), in_file.end());

    std::string out_file_a = config_object.get_string("out_file_a");
    std::string out_file_n = config_object.get_string("out_file_n");


    int reader_num = config_object.get_int("num_threads_processing");
    int merger_num = config_object.get_int("num_threads_merging");

    QVector<std::map<std::string, int>> final_res;

    QVector<std::string> text_results;


    for (boost::filesystem::directory_entry& entry : boost::filesystem::recursive_directory_iterator(in_file)) {
        auto elem = entry.path();
        std::string res;

        if (FileProcessor::is_archive(elem)) {
            FileProcessor::process_archive(elem.string(), res);

            text_results.push_back(res);
        } else if (FileProcessor::is_text(elem)) {
            FileProcessor::process_file(elem.string(), res);
            text_results.push_back(res);
        }

        if (text_results.size() == 1000) {

            final_res.push_back(QtConcurrent::mappedReduced(text_results, process_text, MapProcessor::merge_maps));
            text_results.clear();
        }
    }
    final_res.push_back(QtConcurrent::mappedReduced(text_results, process_text, MapProcessor::merge_maps));
    text_results.clear();

    std::map<std::string, int> final_res_merged = QtConcurrent::mappedReduced(final_res, do_nothing, MapProcessor::merge_maps);


    MapProcessor::write_to_file_alphabetic(out_file_a, final_res_merged);
    MapProcessor::write_to_file_quantity(out_file_n, final_res_merged);
    auto finish = get_current_time_fenced();

    long long time = to_us(finish-start);
    std::cout << "T: " << time << "us" << std::endl;
    std::cout << "T: " << time/1000000 << "s" << std::endl;

    return 0;

}
