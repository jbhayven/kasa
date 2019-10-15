#include <algorithm>
#include <iostream>
#include <vector>
#include <limits>
#include <string>
#include <regex>
#include <cmath>


// ticket information(pair<name, expiration_time>)  
using ticket_info = std::pair<std::string, int>;
// pair<A, B>
// A: An array with ticket information, the ticket position is it's ID.
// B: An array of vectors. Each of the vectors represents best possible
// price for a given time(vector[i][k] = pair<best price for i + 1 tickets
// and the time equals to k, id of the latest used ticket to produce the price>).
using tickets_data = std::pair<std::vector<ticket_info >, std::vector<std::vector<std::pair<int, int> > > >;

// Structure representing information about a route as a vector
// of pairs representing consecutive stops on the route (of type string)
// with given arrival time (in minutes since midnight).
using route_info = std::vector< std::pair<std::string, int> >;

using routes_data = int;//TODO: change to your data type


// In the task the longest trip can take 927 minutes. 
const int MAX_TRIP_LENGTH = 927;

// For the purpose of 'add_new_ticket' and 'get_optimal_ticket_set'
// function it is easier to take a small margin(at least 2) so that
// we don't have to worry about exceeding the vector size.
const int TICKET_SET_SIZE = 1000;

const std::string NEW_ROUTE_REGEX = "[0-9]+( (5:5[5-9]|([6-9]|1[0-9]|20):[0-5][0-9]|21:([0-1][0-9]|2[0-1])) [_\\^A-Za-z]+)*";
const std::string NEW_TICKET_REGEX = "[A-Za-z][ A-Za-z]* [1-9][0-9]*\\.[0-9]{2} [1-9][0-9]*";
const std::string BEST_TICKET_SET_REGEX = "\\?( [_\\^A-Za-z]+ [0-9]+)+ [_\\^A-Za-z]+";

const std::regex NUMBER_REGEX("[0-9]+");
const std::regex STOP_NAME_REGEX("[a-zA-Z\\^\\_]+");
const std::regex TICKET_NAME("[a-zA-Z\\s]+[0-9]?");

// ticket part

/**
 *  Initializes ressources for 'add_new_ticket' and 'get_optimal_ticket_set'
 *  functions. 
 */
void initialize_optimal_ticket_set(tickets_data& t_data) {
    std::pair<int, int> default_pair = std::make_pair(INT_MAX, -1);

    t_data.second.resize(3);

    for (int i = 0; i < 3; i++) {
        std::vector<std::pair<int, int> > row;
        row.resize(MAX_TRIP_LENGTH, default_pair);
        t_data.second[i] = row;
        t_data.second[i][0] = std::make_pair(0, -1);
    }
}

/**
 * Adds a new ticket to the ticket set.
 * 
 * @param ticket_name       Name of the ticket.
 * @param price             Price of the ticket.
 * @param expiration_time   Time before the ticket expires(in minutes).
 * 
 * @note    Complexity O(N) where N is the TICKET_SET_SIZE.
 */
bool add_new_ticket(tickets_data& data, const std::string& ticket_name, const int price, int expiration_time) {

    std::vector<ticket_info> tickets = data.first;
    std::vector<std::vector<std::pair<int, int> > > optimal_ticket_set = data.second;

    for (size_t i = 0; i < tickets.size(); i++)
        if (ticket_name.compare(tickets[i].first) == 0)
            return false;   // Ticket name is identical to some other ticket.

    // Limits the espiration time.
    if (expiration_time > MAX_TRIP_LENGTH)
        expiration_time = MAX_TRIP_LENGTH;

    // Adds ticket to the list and obtains it's id.
    int id = tickets.size();
    tickets.push_back(std::make_pair(ticket_name, expiration_time));

    // Updates the best prices for trips using only one ticket.
    for (int i = expiration_time; i > 0; i--) {
        if (optimal_ticket_set[0][i].first > price)
            optimal_ticket_set[0][i] = std::make_pair(price, id);
        else
            break;
    }

    // Updates the best prices for trips using at least two tickets.
    for (int i = 1; i < 3; i++)
        for (size_t j = expiration_time + 1; j <= MAX_TRIP_LENGTH; j++) {
            int tickets_price = (int)std::min((long long)INT_MAX, price + (long long) optimal_ticket_set[i - 1][j - expiration_time].first);

            if (optimal_ticket_set[i][j].first > tickets_price)
                optimal_ticket_set[i][j] = std::make_pair(tickets_price, id);
        }

    return true;
}

/**
 * Calculates the cheapest possible ticket set
 * for the given trip length.
 * 
 * @param   Length of the trip.
 * 
 * @return  A vector containing names of the tickets that
 *      can last for the length of the trip and are the cheapest.
 *      If there is no such a set, or the trip length is less or equal to zero,
 *      or it is greater than the MAX_TRIP_LENGTH; an empty
 *      vector is returned. 
 * 
 * @note    Complexity O(1).
 */
std::vector<std::string> get_optimal_ticket_set(tickets_data& data, const int trip_length) {

    // A vector containing ticket data
    std::vector<ticket_info> tickets = data.first;
    
    // An array of vectors. Each of the vectors represents best possible
    // price for a given time(vector[i][k] = pair<best price for i + 1 tickets
    // and the time equals to k, id of the latest used ticket to produce the price>).
    std::vector<std::vector<std::pair<int, int> > > optimal_ticket_set = data.second;

    std::vector<std::string> out;

    // Checks whether the trip length(time) is within the limits.
    if (trip_length > MAX_TRIP_LENGTH || trip_length <= 0)
        return out;

    int tickets_count = 0;
    int best_price = INT_MAX;

    // Finds how many tickets to buy.
    for (int i = 0; i < 3; i++) {
        if (optimal_ticket_set[i][trip_length].first < best_price) 
            tickets_count = i + 1;
        best_price = std::min(best_price, optimal_ticket_set[i][trip_length].first);
    }

    // Checks whether there exist at least one valid
    // set of the tickets that can last for the trip.
    if (tickets_count == 0) 
        return out;

    // Obtains names of the tickets from the set.
    int pos = trip_length;
    while (pos > 0) {
        int next_id = optimal_ticket_set[tickets_count - 1][pos].second;

        out.push_back(tickets[next_id].first);
        pos -= tickets[next_id].second;
        tickets_count--;
    }

    return out;
}

/**
 * Checks whether the ticket set was found.
 */
bool ticket_set_found(std::vector<std::string> tickets) {
    if (tickets.size() == 0)
        return false;
    return true;
}

//route part

bool add_new_route(routes_data& data, int route_number, const route_info& stops_on_route) {

    //TODO: swap with the propper method

    return true;
}

bool plan_tickets(routes_data& data, tickets_data& tickets, const std::vector<std::string>& stops, const std::vector<int>& routes) {

    //TODO: swap with the propper method

    return true;
}


//Parser part

void report_error(std::string& txt, int line_num) {
    std::cout << "Error: Wrong input at line " << line_num << ": \"" << txt << "\"\n";
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

/**
 *  Converts input to a valid format for the new route function.
 *  And then invokes it with the given input.
 */
bool parse_and_run_new_route(routes_data& r_data, const std::string text) {

    int route_number;
    std::vector<int> times;
    std::vector<std::string> stops;

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


    // Invokes the function
    if (!add_new_route(r_data, route_number, info))
        return false;

    return true;
}

/**
 *  Converts input to a valid format for the new ticket function.
 *  And then invokes the function with the given input.
 */
bool parse_and_run_new_ticket(tickets_data& t_data, const std::string& text) {

    auto name_begin = std::sregex_iterator(text.begin(), text.end(), TICKET_NAME);
    auto num_begin = std::sregex_iterator(text.begin(), text.end(), NUMBER_REGEX);

    std::string ticket_name = (*name_begin).str();
    ticket_name = ticket_name.substr(0, ticket_name.size() - 2);
    int price = regex_int(num_begin) * 100;
    num_begin++;
    price += regex_int(num_begin);
    num_begin++;
    int expiration_time = regex_int(num_begin);

    //Invokes the function.
    if (!add_new_ticket(t_data, ticket_name, price, expiration_time))
        return false;

    return true;
}

/**
 *  Converts input to a valid format for the best ticket set function.
 *  And then invokes the function with the given input.
 */
bool parse_and_run_best_ticket_set(routes_data& r_data, tickets_data& t_data, const std::string& text) {

    std::vector<int> routes;
    std::vector<std::string> stops;

    auto nums_begin = std::sregex_iterator(text.begin(), text.end(), NUMBER_REGEX);
    auto nums_end = std::sregex_iterator();

    for (std::sregex_iterator i = nums_begin; i != nums_end; ++i)
        routes.push_back(regex_int(i));

    auto stop_begin = std::sregex_iterator(text.begin(), text.end(), STOP_NAME_REGEX);
    auto stop_end = std::sregex_iterator();

    for (std::sregex_iterator i = stop_begin; i != stop_end; ++i)
        stops.push_back((*i).str());

    // Invokes the function.
    if (!plan_tickets(r_data, t_data, stops, routes))
        return false;

    return true;
}

/**
 *  Checks whether a line is compatible with the regex.
 */
bool check_line(const std::string& text, const std::string& reg) {
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
void process_line(routes_data& r_data, tickets_data& t_data, std::string& line, int line_num) {
    bool err = false;

    if (check_line(line, NEW_ROUTE_REGEX))
        err |= !parse_and_run_new_route(r_data, line);
    else if (check_line(line, NEW_TICKET_REGEX))
        err |= !parse_and_run_new_ticket(t_data, line);
    else if (check_line(line, BEST_TICKET_SET_REGEX))
        err |= !parse_and_run_best_ticket_set(r_data, t_data, line);
    else
        report_error(line, line_num + 1);

    if (err)
        report_error(line, line_num + 1);
}

int main() {

    std::string line;
    int line_num = 0;

    routes_data r_data;
    tickets_data t_data;

    initialize_optimal_ticket_set(t_data);

    while (!std::cin.eof()) {
        line = "";

        std::getline(std::cin, line);

        if (line.size() != 0)
            process_line(r_data, t_data, line, line_num);

        line_num++;
    }


    return 0;
}