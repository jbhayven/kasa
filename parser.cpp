#include <algorithm>
#include <iostream>
#include <cmath>
#include <string>
#include <regex>

#define ERROR(txt, line_num) cout << "Error: Wrong input at line " << line_num << ": \"" << txt << "\"\n";

using namespace std;

const string NEW_ROUTE_REGEX = "[0-9]+( (5:5[5-9]|([6-9]|1[0-9]|20):[0-5][0-9]|21:([0-1][0-9]|2[0-1])) [_\\^A-Za-z]+)*";
const string NEW_TICKET_REGEX = "[A-Za-z][ A-Za-z]* [1-9][0-9]*\\.[0-9]{2} [1-9][0-9]*";
const string BEST_TICKET_SET_REGEX = "\\?( [_\\^A-Za-z]+ [0-9]+)+ [_\\^A-Za-z]+";

const std::regex NUMBER_REGEX("[0-9]+");
const std::regex STOP_NAME_REGEX("[a-zA-Z\\^\\_]+");
const std::regex TICKET_NAME("[a-zA-Z\\s]+[0-9]?");

// Structure representing information about a route as a vector
// of pairs representing consecutive stops on the route (of type string)
// with given arrival time (in minutes since midnight).
using route_info = std::vector< std::pair<std::string, int> >;

bool add_new_route(int route_number, const route_info& stops_on_route) {

    //TODO: swap with the propper method

    return true;
}

bool plan_tickets(const std::vector<std::string>& stops, const std::vector<int>& routes) {

    //TODO: swap with the propper method

    return true;
}

/**
 *  Obtains int value from a regex pointer.
 */
int regex_int(std::sregex_iterator ptr) {
    return std::stoi((*ptr).str());
}

/**
 *  Obtains a ticket name from a regex pointer.
 */
std::string regex_ticket_name(std::sregex_iterator ptr) {
    std::string out = (*ptr).str();
    int format = 0;

    if (out[0] == ' ')
        format += 2;
    if (out[out.size() - 1] == ' ')
        format += 1;

    return out.substr(format >> 1, out.size() - (format >> 1) - format % 2);
}

//TODO: Remove(debug function)
void print_str_tab(std::vector<string> &vec) {

    if (vec.size() == 0) {
        std::cout << "Vector is empty!\n";
        return;
    }

    std::cout << "Vector size: " << vec.size() << "\n";
    std::cout << vec[0];

    for (size_t i = 1; i < vec.size(); i++)
        std::cout << ", " << vec[i];
    std::cout << "\n";
}

//TODO: Remove(debug function)
void print_int_tab(std::vector<int> &vec) {

    if (vec.size() == 0) {
        std::cout << "Vector is empty!\n";
        return;
    }

    std::cout << "Vector size: " << vec.size() << "\n";
    std::cout << vec[0];

    for (size_t i = 1; i < vec.size(); i++)
        std::cout << ", " << vec[i];

    std::cout << "\n";
}

/**
 *  Converts input to a valid format for the new route function.
 *  And then invokes it with the given input.
 */
bool parse_and_run_new_route(const std::string text) {

    int route_number;
    std::vector<int> times;
    std::vector<string> stops;

    auto nums_begin = std::sregex_iterator(text.begin(), text.end(), NUMBER_REGEX);
    auto nums_end = std::sregex_iterator();

    route_number = std::stoi((*nums_begin).str());
    nums_begin++;

    int iter = 0;
    for (std::sregex_iterator i = nums_begin; i != nums_end; ++i) {
        if (iter % 2 == 0)
            times.push_back(regex_int(i) * 60);
        else
            times[iter / 2]  += regex_int(i);

        iter++;
    }

    std::cout << "Iter: " << iter << "\n";

    auto name_begin = std::sregex_iterator(text.begin(), text.end(), STOP_NAME_REGEX);
    auto name_ends = std::sregex_iterator();

    for (std::sregex_iterator i = name_begin; i != name_ends; ++i)
        stops.push_back((*i).str());

    if (times.size() != stops.size())
        return false; // Wrong stop/time count.

    //Loads data to the container
    route_info info;
    info.reserve(stops.size());
    for (size_t i = 0; i < times.size(); i++)
        info.push_back(std::make_pair(stops[i], times[i]));


    //TODO: update new route!
    //TODO: check

    if (!add_new_route(route_number, info))
        return false;

    //END OF TODO

    //DEBUG:
    std::cout << "Route: " << route_number << "\n";
    print_int_tab(times);
    print_str_tab(stops);

    return true;
}

/**
 *  Converts input to a valid format for the new ticket function.
 *  And then invokes the function with the given input.
 */
bool parse_and_run_new_ticket(const std::string text) {

    auto name_begin = std::sregex_iterator(text.begin(), text.end(), TICKET_NAME);
    auto num_begin = std::sregex_iterator(text.begin(), text.end(), NUMBER_REGEX);

    std::string ticket_name = (*name_begin).str();
    ticket_name = ticket_name.substr(0, ticket_name.size() - 2);
    int price = regex_int(num_begin) * 100;
    num_begin++;
    price += regex_int(num_begin);
    num_begin++;
    int expiration_time = regex_int(num_begin);

    //TODO: update new ticket

    //END OF TODO

    //DEBUG
    std::cout << "Ticket name: " << ticket_name << "\n";
    std::cout << "Price: " << price << "\n";
    std::cout << "Expiration: " << expiration_time << "\n";

    return true;
}

/**
 *  Converts input to a valid format for the best ticket set function.
 *  And then invokes the function with the given input.
 */
bool parse_and_run_best_ticket_set(const std::string text) {

    std::vector<int> routes;
    std::vector<string> stops;

    auto nums_begin = std::sregex_iterator(text.begin(), text.end(), NUMBER_REGEX);
    auto nums_end = std::sregex_iterator();

    for (std::sregex_iterator i = nums_begin; i != nums_end; ++i)
        routes.push_back(regex_int(i));

    auto stop_begin = std::sregex_iterator(text.begin(), text.end(), STOP_NAME_REGEX);
    auto stop_end = std::sregex_iterator();

    for (std::sregex_iterator i = stop_begin; i != stop_end; ++i)
        stops.push_back((*i).str());

    //TODO: update ticket set
    //TODO: check

    if (!plan_tickets(stops, routes))
        return false;


    //ENDO OF TODO

    //Debug
    std::cout << "Hello my deer!\n";
    print_int_tab(routes);
    print_str_tab(stops);

    return true;
}

/**
 *  Checks whether a line is compatible with the regex.
 */
bool check_line(const std::string &text, const std::string &reg) {
    std::regex word_regex(reg);
    auto words_begin = std::sregex_iterator(text.begin(), text.end(), word_regex);
    auto words_end = std::sregex_iterator();

    if (words_begin == words_end)
        return false;

    std::smatch match = *words_begin;
    std::string match_str = match.str();

    if (match_str.size() == text.size())
        return true;
    return false;
}

/**
 *  Checks if the line is in propper format and
 *  if so invokes a corresponding function.
 */
void process_line(string line, int line_num) {
    bool err = false;

    if (check_line(line, NEW_ROUTE_REGEX))
        err |= !parse_and_run_new_route(line);
    else if (check_line(line, NEW_TICKET_REGEX))
        err |= !parse_and_run_new_ticket(line);
    else if (check_line(line, BEST_TICKET_SET_REGEX))
        err |= !parse_and_run_best_ticket_set(line);
    else
        ERROR(line, line_num + 1);

    if (err)
        ERROR(line, line_num + 1);
}

int main() {

    string line;
    int line_num = 0;

    while (!std::cin.eof()) {
        line = "";

        getline(std::cin, line);

        if (line.size() != 0)
            process_line(line, line_num);

        line_num++;
    }


    return 0;
}
