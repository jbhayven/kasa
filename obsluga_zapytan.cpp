#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <unordered_set>
#include <vector>
#include <string>
#include <utility>

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

//do usunięcia, potrzebne do testów
std::vector< std::string> optimal_ticket_set(int time){
    return {"DZIENNY", "NOWY", std::to_string(time)};
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
    
/**
 * Checks whether the ticket set was found.
 */
bool ticket_set_found(std::vector<std::string> tickets) {
    if (tickets.size() == 0)
        return false;
    return true;
}

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
                  const bus_schedule& schedule)
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
    
    std::vector< std::string > optimal_tickets = optimal_ticket_set(trip_time);

    if(ticket_set_found(optimal_tickets) == false) {
        std::cout << ":|" << std::endl;
    }
    else{
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

    // do usunięcia, dodane na potrzeby testowania
    #include <cassert>
int main(){

    // A map storing information about arrival time for every bus route 'R'
    // and every bus stop 'S' utilized by such route. 
    // Such time can be accessed as a value for pair <S, R>, which
    // constitutes a 'schedule point'.
    bus_schedule schedule;
    
    // A set storing all route IDs already existing in the system.
    std::set<int> existing_routes;
    
    using namespace std;
    
    route_info route1 = { {"WARSZAWA", 10}, {"LUBLIN", 92} };
    route_info route2 = { {"KRAKÓW", 11}, {"WROCŁAW", 92} };
     
    
    assert(add_new_route(1, route1, existing_routes, schedule));
    assert(!add_new_route(1, route2, existing_routes, schedule));
    
    route_info route3 = { {"SZCZECIN", 13}, {"GDAŃSK", 11} };
    
    assert(!add_new_route(1, route3, existing_routes, schedule));
    
    route_info route4 = { {"LUBLIN", 92}, {"BOSTON", 121210} };
    route_info route5 = { {"LUBLIN", 91}, {"BOSTON", 121210} };
    route_info route6 = { {"LUBLIN", 93}, {"BOSTON", 121210} };
    
    assert(add_new_route(4, route4, existing_routes, schedule));
    assert(add_new_route(5, route5, existing_routes, schedule));
    assert(add_new_route(6, route6, existing_routes, schedule));
    
    vector<std::string> stops = {"WARSZAWA", "LUBLIN", "BOSTON"};
    
    vector<int> routes1 = {1, 4};
    vector<int> routes2 = {1, 5};
    vector<int> routes3 = {1, 6};
    
    assert(check_trip_validity(stops, routes1, schedule));
    assert(!check_trip_validity(stops, routes2, schedule));
    assert(check_trip_validity(stops, routes3, schedule));
    
    assert(plan_tickets(stops, routes1, schedule));
    assert(!plan_tickets(stops, routes2, schedule));
    assert(plan_tickets(stops, routes3, schedule));
    
    vector<std::string> stops2 = {"WARSZAWA", "LUBLIN", "OSTROŁĘKA"};
    assert(!check_trip_validity(stops2, routes1, schedule));
    
    vector<std::string> inv_stops = {"LUBLIN", "WARSZAWA", "BOSTON"};
    assert(!check_trip_validity(inv_stops, routes1, schedule));
    
    return 0;
}
