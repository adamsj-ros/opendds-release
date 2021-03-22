#include "ArgumentParser.h"

#include <util.h>
#include <json_conversion.h>
#include <BenchTypeSupportImpl.h>

#include <unordered_map>

using namespace Bench;

bool ArgumentParser::parse(int argc, ACE_TCHAR* argv[], OutputType& output_type,
    OutputFormat& output_format, Report& report, std::ofstream& output_file_stream,
    ParseParameters& parse_parameters)
{
  std::string input_file_path;
  std::string output_file_path;

  if (argc == 1) {
    show_usage_prompt();
  } else {
    try {
      for (int i = 1; i < argc; i++) {
        const ACE_TCHAR* argument = argv[i];

        if (!ACE_OS::strcmp(argument, ACE_TEXT("--help"))
            || !ACE_OS::strcmp(argument, ACE_TEXT("-h"))) {
          show_usage();
          return false;
        }

        if (!ACE_OS::strcmp(argument, ACE_TEXT("--input-file"))) {
          std::string option_argument = get_option_argument(i, argc, argv);
          input_file_path = option_argument;
        } else if (!ACE_OS::strcmp(argument, ACE_TEXT("--output-file"))) {
          std::string option_argument = get_option_argument(i, argc, argv);
          output_file_path = option_argument;
        } else if (!ACE_OS::strcmp(argument, ACE_TEXT("--output-type"))) {
          std::string option_argument = get_option_argument(i, argc, argv);
          static std::unordered_map<std::string, OutputType> const table = {
            { "time-series", OutputType::TimeSeries }
          };

          auto it = table.find(option_argument);

          if (it != table.end()) {
            output_type = it->second;
          } else {
            show_option_argument_error(option_argument);
            return false;
          }
        } else if (!ACE_OS::strcmp(argument, ACE_TEXT("--output-format"))) {
          std::string option_argument = get_option_argument(i, argc, argv);
          static std::unordered_map<std::string, OutputFormat> const table = {
            { "gnuplot", OutputFormat::Gnuplot }
          };

          auto it = table.find(option_argument);

          if (it != table.end()) {
            output_format = it->second;
          } else {
            show_option_argument_error(option_argument);
            return false;
          }
        } else if (!ACE_OS::strcmp(argument, ACE_TEXT("--tags"))) {
          if (!parse_parameters.tags.empty()) {
            continue;
          }

          if (i + 1 < argc) {
            for (int index = i + 1; index < argc; index++) {
              std::string option = ACE_TEXT_ALWAYS_CHAR(argv[index]);
              if (option[0] == '-') {
                break;
              } else {
                parse_parameters.tags.insert(option);
                i++;
              }
            }
          }

          if (parse_parameters.tags.empty()) {
            std::cout << "Missing tag types";
            return false;
          }
        } else if (!ACE_OS::strcmp(argument, ACE_TEXT("--stats"))) {
          if (!parse_parameters.stats.empty()) {
            continue;
          }

          if (i + 1 < argc) {
            for (int index = i + 1; index < argc; index++) {
              std::string option = ACE_TEXT_ALWAYS_CHAR(argv[index]);
              if (option[0] == '-') {
                break;
              } else {
                parse_parameters.stats.insert(option);
                i++;
              }
            }
          }

          if (parse_parameters.stats.empty()) {
            std::cout << "Missing stat types";
            return false;
          }
        } else if (!ACE_OS::strcmp(argument, ACE_TEXT("--values"))) {
          if (!parse_parameters.values.empty()) {
            continue;
          }

          if (i + 1 < argc) {
            for (int index = i + 1; index < argc; index++) {
              std::string option = ACE_TEXT_ALWAYS_CHAR(argv[index]);
              if (option[0] == '-') {
                break;
              } else {
                parse_parameters.values.insert(option);
                i++;
              }
            }
          }

          if (parse_parameters.values.empty()) {
            std::cout << "Missing value types";
            return false;
          }
        } else {
          show_option_error(ACE_TEXT_ALWAYS_CHAR(argument));
          return false;
        }
      }
    } catch (int) {
      show_usage_prompt();
      return false;
    }
  }

  if (input_file_path.empty()) {
    std::cerr << "Input file not specified" << std::endl;
    return false;
  } else if (output_file_path.empty()) {
    std::cerr << "Output file not specified" << std::endl;
    return false;
  } else {
    std::cout << "Opening input file: " << input_file_path << std::endl;

    std::ifstream ifs(input_file_path);

    if (ifs.is_open()) {
      std::cout << "Reading input file" << std::endl;

      if (!json_2_idl(ifs, report)) {
        std::cerr << "Could not parse " << input_file_path << std::endl;
        return false;
      } else {
        std::cout << "Opening output file: " << output_file_path << std::endl;
        output_file_stream.open(output_file_path);
        if (!output_file_stream.is_open()) {
          std::cerr << "Could not open output file " << output_file_path << std::endl;
          return false;
        }
      }
    } else {
      std::cerr << "Could not open input file " << input_file_path << std::endl;
      return false;
    }
  }

  return true;
}

void ArgumentParser::show_usage_prompt()
{
  std::cerr << "Use -h or --help to see the full help message" << std::endl;
}

void ArgumentParser::show_option_error(std::string option)
{
  std::cerr << "Invalid option: " << option << std::endl;
  show_usage_prompt();
}

void ArgumentParser::show_option_argument_error(std::string option_argument)
{
  std::cerr << "Invalid option argument: " << option_argument << std::endl;
  show_usage_prompt();
}

void ArgumentParser::show_usage()
{
  std::cout
    << "usage: report_parser [-h|--help] | [OPTIONS...]" << std::endl
    << "OPTIONS:" << std::endl
    << "--input-file  <filename>     The report file to read" << std::endl
    << "--output-file <filename>     The parsed file to generate" << std::endl
    << "--output-type <type>         Specifies type of data to parse." << std::endl
    << "     Options:" << std::endl
    << "            time-series:     Parses out time-series data" << std::endl
    << "--output-format <format>     Specifies format of output." << std::endl
    << "     Options:" << std::endl
    << "            gnuplot:         Formats output for plotting with gnuplot" << std::endl
    << "--tags   <list of tags>      Specifies the status block types to output." << std::endl
    << "--stats  <list of stats>     Specifies the status block value types to output." << std::endl
    << "--values <list of values>    Specifies the status block values to output." << std::endl;
}
