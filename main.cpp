
#include <iostream>
#include <fstream> // Include the <fstream> header for file input/output
#include <string>  // Include the <string> header for string manipulation
#include <format>
#include <nlohmann/json.hpp>
#include "subprocess.hpp"

nlohmann::json GetTitleSet(const std::string& pathDVD)
{
    subprocess::Popen process(std::vector<std::string>{"/home/wintermute/HandBrake/build/HandBrakeCLI", "-i", pathDVD, "-t", "0", "--json"}, subprocess::output{subprocess::PIPE}, subprocess::error{subprocess::PIPE});
    std::string contents = process.communicate().first.buf.data();
    const std::string sstring = "JSON Title Set:";
    return nlohmann::json::parse(contents.substr(contents.find(sstring) + sstring.length()));
}

void RipTrack(size_t idx, const std::string& pathDVD, const std::string& pathOutput)
{
    subprocess::Popen process(std::vector<std::string>{"/home/wintermute/HandBrake/build/HandBrakeCLI", "-i", pathDVD, "-t", std::to_string(idx), "-o", pathOutput, "-m", "--audio-lang-list", "deu,eng", "--first-audio", "deu"}, subprocess::output{subprocess::PIPE});
    std::string contents = process.communicate().first.buf.data();
    if(process.retcode() != 0)
        throw std::runtime_error(contents);
}

int main(int, char**)
{
    try
    {
        const auto jsonTitleSet = GetTitleSet("/dev/sr0");
        for(size_t i = 2; i < jsonTitleSet.at("TitleList").size(); ++i)
        {
            RipTrack(i, "/dev/sr0", std::format("/home/wintermute/dev/hb-helper/MNIE_{}.mp4", i-1));
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
    /*
    std::string titleSet;
    {
        // reading contents.json file into a string
        std::ifstream i("/home/wintermute/dev/hb-helper/contents.json");
        std::string contents((std::istreambuf_iterator<char>(i)), std::istreambuf_iterator<char>());
        // find following string in contents.json : "Version:" and split it into two parts and store the second part in a variable
        const std::string sstring = "JSON Title Set:";
        titleSet = contents.substr(contents.find(sstring)+sstring.length());
    }
    nlohmann::json j = nlohmann::json::parse(titleSet);
    size_t i = 0;
    for(const auto& title : j.at("TitleList"))
    {
        if(i == 0)
        {
            ++i;
            continue;
        }

        std::cout << std::format("idx {} : ", ++i) << title.at("Duration") << std::endl;
        // execute a process and create a pipe to read from
        FILE *pipe = popen(std::format("/home/wintermute/HandBrake/build/HandBrakeCLI -i {} -t {} -o /home/wintermute/dev/hb-helper/{}.MP4", std::string(title.at("Path")), i, i).c_str(), "r");
        if (!pipe) {
            std::cerr << "Couldn't start command." << std::endl;
            return 0;
        }
        char buffer[128];
        std::string result = "";
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
        }
        pclose(pipe);
    }
    return 0;*/
}
