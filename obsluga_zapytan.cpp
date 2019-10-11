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

// A map storing information about arrival time for every bus route 'R'
// and every bus stop 'S' utilized by such route. 
// Such time can be accessed as a value for pair <S, R>, which
// constitutes a 'schedule point'.
std::map<schedule_point, int> schedule;

// A set storing all route IDs already existing in the system.
std::set<int> existing_routes;

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
 * 
 * @return  The result of validity check. 
 */
bool is_valid_new_route(int route_number, const route_info& stops_on_route) {
    if(existing_routes.count(route_number) > 0)
        return false;
    
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
 * 
 * @return  False if incorrect arguments (in which case nothing was added).
 *          Otherwise true.
 */
bool add_new_route(int route_number, const route_info& stops_on_route) {
    if(is_valid_new_route(route_number, stops_on_route) == false) return false;
    for(auto i = stops_on_route.begin(); i != stops_on_route.end(); i++) {
        schedule[create_schedule_point(route_number, (*i).first)] = (*i).second;
    }
    return true;
}

/**
 * @short Checks whether a schedule_point doesn't exist in the schedule.
 * 
 * Checks whether a schedule_point doesn't exist in the schedule.
 * 
 * @param   schedule_point to be tested.
 * 
 * @return  @true if p is not included in the schedule, @false otherwise.
 */
bool not_scheduled(schedule_point p){
    return schedule.count(p) == 0;
}

bool check_trip_validity (const std::vector<std::string>& stops, 
                          const std::vector<int>& routes) 
{
    if(stops.size() < 2) return false;
    if(routes.size() < 1 ) return false;
    
    std::vector<schedule_point> departure_points (routes.size());
    std::transform(routes.begin(), routes.end(), stops.begin(),
                   departure_points.begin(), create_schedule_point);
    if(std::any_of(departure_points.begin(), departure_points.end(), 
                  not_scheduled)) return false;
                  
    std::vector<schedule_point> arrival_points (routes.size());
    std::transform(routes.begin(), routes.end(), ++stops.begin(),
                   arrival_points.begin(), create_schedule_point);
    if(std::any_of(arrival_points.begin(), arrival_points.end(), 
                  not_scheduled)) return false;
                   
    int last_time = schedule.at(arrival_points.front());
    
    for(auto i = departure_points.begin(), j = arrival_points.begin();
        i != departure_points.end(); i++, j++)
    {
        if(schedule.at(*i) < last_time) return false;
        last_time = schedule.at(*i);
         
        if(schedule.at(*j) < last_time) return false;
        last_time = schedule.at(*j);
    }
    return true;
}

//WIP
std::vector< std::string> optimal_ticket_set(int time){
    int b = time;
    std::cout << b << std::endl;
    return {"DZIENNY", "NOWY"};
}

void write_plan(const std::vector<schedule_point>& departure_points,
                const std::vector<schedule_point>& arrival_points, 
                bool no_waiting, std::string where_needs_to_wait)
{    
    if(no_waiting == false){
        std::cout << ":(" << " " << where_needs_to_wait << std::endl;
    }
    
    std::vector< std::string > optimal_tickets =
        optimal_ticket_set(schedule.at(arrival_points.back()) - 
                           schedule.at(departure_points.front()));

    if(optimal_tickets.size() == 0) {
        std::cout << ":|" << std::endl;
    }
    else{
        std::cout << "!" << " " << optimal_tickets.front();
        for(auto i = ++optimal_tickets.begin(); i != optimal_tickets.end(); i++){
            std::cout << ";" << " " << (*i);
        }
        std::cout << std::endl;
    }
}

/**
 * Answers requests to find the optimal ticket buying strategy
 * by writing the results to the standard output.
 * The requests need to be formally correct, in other words – the function
 * is_valid_trip, if called on the same parameters, must return true.
 * 
 * @param   vector of stops to visit in the order of visit (at least 2)
 * @param   vector of routes used to travel between consecutive pairs of stops, in the respective order (at least 1)
 *
 * @return @param false if the request was invalid. @param true otherwise.
 */
bool plan_tickets (const std::vector<std::string>& stops, 
                   const std::vector<int>& routes) 
{
    if(check_trip_validity(stops, routes) == false) return false;
    
    std::vector<schedule_point> departure_points (routes.size());
    std::transform(routes.begin(), routes.end(), stops.begin(),
                   departure_points.begin(), create_schedule_point);
                   
    std::vector<schedule_point> arrival_points (routes.size());
    std::transform(routes.begin(), routes.end(), ++stops.begin(),
                   arrival_points.begin(), create_schedule_point);
                   
    int last_time = schedule.at(arrival_points.front());
    bool no_waiting = true;
    std::string where_needs_to_wait;
    
    for(auto i = departure_points.begin(), j = arrival_points.begin();
        i != departure_points.end(); i++, j++)
    {
        if(schedule.at(*i) != last_time) {
            no_waiting = false;
            where_needs_to_wait = (*i).first;
        }
         
        last_time = schedule.at(*j);
    }
    
    write_plan(departure_points, arrival_points, no_waiting, where_needs_to_wait);
    return true;
}

int main(){
    // [TODO] testy i debug
    return 0;
}
