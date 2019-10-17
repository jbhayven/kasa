
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>
#include <limits>
#include <string>
#include <regex>
#include <cmath>
#include <map>
#include <set>


//ticket information(pair<name, expiration_time>)  
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

// Structure representing a pair <S, R> of a bus stop S
// lying on the route R.
using schedule_point = std::pair<std::string, int>;

// Structure mapping timestamps to schedule_points 
// represented by pairs <bus_stop, bus_route>, containing 
// all information about arrival times of all bus routes for every stop.
using bus_schedule = std::map<schedule_point, int>;

//TODO: doc
using routes_data = std::pair<std::set<int>, bus_schedule>;


// In the task the longest trip can take 927 minutes. 
const int MAX_TRIP_LENGTH = 927;

// For the purpose of 'add_new_ticket' and 'get_optimal_ticket_set'
// function it is easier to take a small margin(at least 2) so that
// we don't have to worry about exceeding the vector size.
const int TICKET_SET_SIZE = 1000;

const std::string NEW_ROUTE_REGEX = "[0-9]+( (5:5[5-9]|([6-9]|1[0-9]|20):[0-5][0-9]|21:([0-1][0-9]|2[0-1])) [_\\^A-Za-z]+)*";
const std::string NEW_TICKET_REGEX = "[ A-Za-z]* [1-9][0-9]*\\.[0-9]{2} [1-9][0-9]*";
const std::string BEST_TICKET_SET_REGEX = "\\?( [_\\^A-Za-z]+ [0-9]+)+ [_\\^A-Za-z]+";

const std::regex NUMBER_REGEX("[0-9]+");
const std::regex STOP_NAME_REGEX("[a-zA-Z\\^\\_]+");
const std::regex TICKET_NAME("[ a-zA-Z]+[0-9]?");

// ticket part

/**
 *  Initializes ressources for 'add_new_ticket' and 'get_optimal_ticket_set'
 *  functions. 
 * 
 * @param t_data            The ticket data that will be initialized.
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

    std::vector<ticket_info>& tickets = data.first;
    std::vector<std::vector<std::pair<int, int> > >& optimal_ticket_set = data.second;

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
std::vector<std::string> optimal_ticket_set(const tickets_data& t_data, const int trip_length) {

    // A vector containing ticket data
    const std::vector<ticket_info>& tickets = t_data.first;
    
    // An array of vectors. Each of the vectors represents best possible
    // price for a given time(vector[i][k] = pair<best price for i + 1 tickets
    // and the time equals to k, id of the latest used ticket to produce the price>).
    const std::vector<std::vector<std::pair<int, int> > >& optimal_ticket_set = t_data.second;

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

/**
 * @short Checks the validity of a request of the first type.
 * 
 * Checks whether given data can constitute a valid request
 * to add a new route to the system. 
 * A valid request must satisfy the following criteria:
 * 1) A route of given number must not already exist in the system;
 * 2) No stop occurs twice on the route;
 * 3) All arrival times are given in an increasing order.
 * 
 * @param   number (unique) of the route to be added
 * @param   a vector of pairs <stop_name, arrival_time> describing the new route
 * @param   the set holding the IDs of all existing bus routes
 * 
 * @return  The result of validity check. 
 */
bool is_valid_new_route(int route_number, const route_info& stops_on_route,
                        const std::set<int>& existing_routes) 
{
    if(existing_routes.count(route_number) > 0)
        return false;
        
    if(stops_on_route.size() == 0) return false;
    
    std::unordered_set<std::string> visited_stops;
    int last_stop_time = 0;
    
    for(auto i = stops_on_route.begin(); i != stops_on_route.end(); i++) {
        visited_stops.insert((*i).first);
        
        if((*i).second <= last_stop_time) return false;
        last_stop_time = (*i).second;
    }
    
    if(visited_stops.size() < stops_on_route.size()) return false;
    
    return true;
}

/**
 * @short Creates a new schedule_point object.
 * 
 * Creates a new schedule_point object based on given parameters.
 * 
 * @param   number of a bus route
 * @param   name of a bus stop lying on the route
 * 
 * @return  The created schedule_point. 
 */
schedule_point create_schedule_point(int route, std::string bus_stop) {
    return make_pair(bus_stop, route);
}

/**
 * @short Adds a new route
 * 
 * Fulfils a request to add a new route according to the parameters.
 * 
 * @param   number (unique) of the route to be added
 * @param   a vector of pairs <stop_name, arrival_time> describing the new route  
 * @param   the set holding the IDs of all existing bus routes
 * @param   the structure representing all existing relations of bus stops 
 *          and routes in the form of bus schedule
 * 
 * @return  False if given arguments do not constitute a valid new route 
 *          (in which case nothing is added). Otherwise true.
 */
bool add_new_route(int route_number, const route_info& stops_on_route, 
                   std::set<int>& existing_routes, 
                   bus_schedule& schedule) 
{
    if(is_valid_new_route(route_number, stops_on_route, 
                          existing_routes) == false) return false;
                          
    existing_routes.insert(route_number);
    for(auto i = stops_on_route.begin(); i != stops_on_route.end(); i++) {
        schedule[create_schedule_point(route_number, (*i).first)] = (*i).second;
    }
    return true;
}

bool contains(const bus_schedule& schedule, schedule_point k){
    return schedule.count(k) > 0;
}

/**
 * @short Checks the validity of a request of the third type.
 * 
 * Checks whether given data can constitute a valid request to find the
 * optimal ticket buying strategy.
 * A valid request must satisfy the following criteria:
 * 1) The number of stops given must be equal to the number of connecting
 *    routes given + 1; there need to be at least one connection, 
 *    which constitutes a minimum of two stops and one route.
 * 2) All given routes must exist in the schedule and include respective stops.
 * 3) Given order of stops and routes must imply a non-decreasing order 
 *    of the respective arrival and departure times, according to the schedule.
 * 
 * @param   a vector containing a sequence of stops
 * @param   a vector containing the sequence of routes intended to use 
 *          for travel between the stops
 * @param   the structure representing all existing relations of bus stops 
 *          and routes in the form of bus schedule
 * 
 * @return  The result of validity check. 
 */
bool check_trip_validity (const std::vector<std::string>& stops, 
                          const std::vector<int>& routes,
                          const bus_schedule& schedule) 
{
    if(stops.size() < 2) return false;
    if(routes.size() < 1 ) return false;
    if(stops.size() != routes.size() + 1) return false;

    std::vector<schedule_point> trip_points;
    
    auto curr_route = routes.begin();
    for(auto departing_stop = stops.begin(), target_stop = ++stops.begin();
        curr_route != routes.end();
        departing_stop++, target_stop++, curr_route++)
    {
        trip_points.push_back(create_schedule_point(*curr_route, 
                                                    *departing_stop));
        trip_points.push_back(create_schedule_point(*curr_route, 
                                                    *target_stop));
    }
    
    for(auto i = trip_points.begin(), j = ++trip_points.begin();
        j != trip_points.end(); 
        i++, j++)
    {
        if(schedule.count(*i) == 0) return false;
        if(schedule.count(*j) == 0) return false;
        if(schedule.at(*i) > schedule.at(*j)) return false;
    }
    
    return true;
}

/**
 * Performs checks necessary to find the optimal ticket buying strategy
 * for a request of the third type or conclude that it is not possible.
 * Returns the results of the processing as a tuple.
 * 
 * @param   vector of stops to visit in the order of visit
 * @param   vector of routes used to travel between consecutive pairs of stops, in the respective order
 * @param   the structure representing all existing relations of bus stops 
 *          and routes in the form of bus schedule
 * 
 * @return  A tuple containing the result of the checks, whose 
 *          first element denotes the time the entire trip would take,
 *          second element is true if and only if it is required to wait
 *          at least once during the trip, 
 *          third element is equal to the name of the first stop of the trip
 *          where waiting would be required (if the second element is true),
 *          and an empty string otherwise
 */
std::tuple<int, bool, std::string> 
    scan_trip_request(const std::vector<std::string>& stops, 
                      const std::vector<int>& routes, 
                      const bus_schedule& schedule) 
{
    std::vector<schedule_point> departure_points (routes.size());
    std::transform(routes.begin(), routes.end(), stops.begin(),
                   departure_points.begin(), create_schedule_point);
                   
    std::vector<schedule_point> arrival_points (routes.size());
    std::transform(routes.begin(), routes.end(), ++stops.begin(),
                   arrival_points.begin(), create_schedule_point);
                   
    int last_time = schedule.at(departure_points.front());
    bool needs_waiting = false;
    std::string where_needs_to_wait = "";
    
    for(auto i = departure_points.begin(), j = arrival_points.begin();
        i != departure_points.end(); i++, j++)
    {
        if(needs_waiting == false && schedule.at(*i) != last_time) {
            needs_waiting = true;
            where_needs_to_wait = (*i).first;
        }
         
        last_time = schedule.at(*j);
    }
    
    int travel_time = schedule.at(arrival_points.back()) - 
                           schedule.at(departure_points.front());
    
    return std::make_tuple(travel_time, needs_waiting, where_needs_to_wait);
}

// route part

/**
 * Provides an answer for a third type request. Writes the result 
 * to standard output, according to the task specification 
 * at https://moodle.mimuw.edu.pl/mod/assign/view.php?id=19131.
 * 
 * @param   vector of stops to visit in the order of visit
 * @param   vector of routes used to travel between consecutive pairs of stops, 
 *          in the respective order
 * @param   the structure representing all existing relations of bus stops 
 *          and routes in the form of bus schedule
 *
 * @return False if the request was invalid. True otherwise.
 */
bool plan_tickets(const std::vector<std::string>& stops, 
                  const std::vector<int>& routes, 
                  const bus_schedule& schedule,
                  const tickets_data& t_data,
                  int& tickets_sold)
{    
    if(check_trip_validity(stops, routes, schedule) == false) return false;
    
    int trip_time;
    bool waits;
    std::string where_waits;
    std::tie (trip_time, waits, where_waits) = 
        scan_trip_request(stops, routes, schedule);
    
    if(waits == true){
        std::cout << ":(" << " " << where_waits << std::endl;
        return true;
    }

    std::vector< std::string > optimal_tickets = optimal_ticket_set(t_data, trip_time + 1);

    if(ticket_set_found(optimal_tickets) == false) {
        std::cout << ":|" << std::endl;
    }
    else{
        tickets_sold += optimal_tickets.size();

        std::cout << "!" << " " << optimal_tickets.front();
        
        for(auto i = ++optimal_tickets.begin(); 
            i != optimal_tickets.end(); 
            i++)
        {
            std::cout << ";" << " " << (*i);
        }
        
        std::cout << std::endl;
    }
    return true;
}


//Parser part

void report_error(std::string& txt, int line_num) {
    std::cerr << "Error in line " << line_num << ":" << txt << "\n";
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


    // // Invokes the function
    // if (!add_new_route(r_data, route_number, info))
    //     return false;

    return add_new_route(route_number, info, r_data.first, r_data.second);
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
bool parse_and_run_plan_tickets(routes_data& r_data, tickets_data& t_data, int& tickets_sold, const std::string& text) {

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
    if (!plan_tickets(stops, routes, r_data.second, t_data, tickets_sold))
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
void process_line(routes_data& r_data, tickets_data& t_data, int& tickets_sold, std::string& line, int line_num) {
    bool err = false;

    if (check_line(line, NEW_ROUTE_REGEX))
        err |= !parse_and_run_new_route(r_data, line);
    else if (check_line(line, NEW_TICKET_REGEX))
        err |= !parse_and_run_new_ticket(t_data, line);
    else if (check_line(line, BEST_TICKET_SET_REGEX))
        err |= !parse_and_run_plan_tickets(r_data, t_data, tickets_sold, line);
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
    int tickets_sold = 0;

    initialize_optimal_ticket_set(t_data);

    while (!std::cin.eof()) {
        line = "";

        std::getline(std::cin, line);

        if (line.size() != 0)
            process_line(r_data, t_data, tickets_sold, line, line_num);

        line_num++;
    }

    std::cout << tickets_sold << "\n";

    return 0;
}