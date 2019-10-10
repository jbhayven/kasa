#include <iostream>
#include <vector>
#include <string>

// Structure representing information about a route as a vector
// of pairs representing consecutive stops on the route (of type string)
// with given arrival time (in minutes since midnight).
using route_info = std::vector< std::pair<std::string, int> >;

// Structure representing a pair <S, R> of a bus stop S
// lying on the route R.
using schedule_point = std::pair<std::string, int>

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
bool is_valid_route(int route_number, const route_info& stops_on_route) {
    if(existing_routes.count(route_number) > 0)
        return false;
    
    std::unordered_set<string> visited_stops;
    int last_stop_time = 0;
    
    for(
        route_info::iterator i = stops_on_route.begin(); 
        i != stops_on_route.end(); i++;
    ) {
        visited_stops.insert((*i).first);
        
        if((*i).second <= last_stop_time) return false;
        last_stop_time = (*i).second;
    }
    
    if(visited_stops.size() < stops_on_route.size()) return false;
    
    return true;
}

/**
 * @short Adds a new route
 * 
 * Fulfils a request to add a new route according to the parameters.
 * The validity of the parameters must be checked 
 * before calling this function.
 * 
 * @param   number (unique) of the route to be added
 * @param   a vector of pairs <stop_name, arrival_time> describing the new route   
 */
void add_new_route(int route_number, const route_info& stops_on_route) {
    for(
        route_info::iterator i = stops_on_route.begin(); 
        i != stops_on_route.end(); i++;
    ) {
        schedule[make_pair(route_number, (*i).first)] = (*i).second;
    }
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
schedule_point create_schedule_point(int route, 
                                     std::string bus_stop)
{
    return make_pair(bus_stop, route);
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

/**
 * @short Сomparator for sorting schedule_points by time.
 * 
 * A comparator used to sort schedule_points by the time they are
 * scheduled for, in order of increasing times.
 * 
 * @param   first schedule_point to be compared
 * @param   second schedule_point to be compared
 * 
 * @return  @true if a is scheduled for a time no later than b, 
 *          @false otherwise.
 */
bool compare_schedule_point_time(schedule_point a, schedule_point b){
    return schedule.at(a) <= schedule.at(b);
}

/**
 * Asserts that each schedule_point in the first series 
 * is scheduled for a time no later than the respective
 * schedule_point in the other series.
 * 
 * @param   iterator to the start of the first series
 * @param   iterator delimiting the first series
 * @param   iterator to the start of the other series
 * 
 * @return  @true if the assertion succeeds, @false otherwise.
 */
bool is_later(std::vector<schedule_point>::iterator earlier_times_start,
              std::vector<schedule_point>::iterator earlier_times_end,
              std::vector<schedule_point>::iterator latter_times_start)
{
    std::vector<bool> comparison_results;
    
    std::transform(earlier_times_start, earlier_times_end, 
                   latter_times_start, comparison_results.begin(),
                   compare_schedule_point_time);                
    
    return !std:: // whether all comparisons yielded true [TODO]
}

/**
 * Checks whether a given request to find the best ticket purchase
 * strategy is valid.
 * Validity constitutes all of the following:
 * 1)   It is possible to travel between every pair of consecutive stops
 *      from the given list using respective bus routes from the other list;
 * 2)   The direction of all connections is valid, i.e. every bus connection
 *      leads from a stop that is earlier on the list to his latter counterpart
 *      and not the other way around;
 * 3)   For every stop the tourist will depart from, the departure time
 *      in the schedule is not earlier than the arrival time
 *      for the same stop.
 * The comparison for (3) doesn't assert equality. This means that 
 * even if the request is valid, a potential traveller may have to wait 
 * at least once on the way. Also, a correct ticket purchase strategy
 * might not exist for the given set of stops and routes. 
 * Both of these have to be checked independently of this function.
 * 
 * @param   vector of stops to visit in the order of visit
 * @param   vector of routes used to travel between consecutive pairs of stops, in the respective order
 * 
 * @return  @true if the request is valid, @false otherwise.
 */
bool is_valid_trip (const std::vector<std::string>& stops, 
                     const std::vector<int>& routes) 
{
    std::vector<schedule_point> departure_points (routes.size());
    departure_points.resize(routes.size());

    std::transform(routes.begin(), routes.end(), stops.begin(),
                   departure_points.begin(), create_schedule_point);
                   
    std::vector<schedule_point> arrival_points (routes.size());
    arrival_points.resize(routes.size());

    std::transform(routes.begin(), routes.end(), ++stops.begin(),
                   arrival_points.begin(), create_schedule_point);
    
    if(is_later(departure_points.begin(), departure_points.end(), 
                arrival_points.begin()) == false) return false;    
    
    if(is_later(++arrival_points.begin(), arrival_points.end(), 
                departure_points.begin()) == false) return false;  
    
    return true;
}

/**
 * Answers requests to find the optimal ticket buying strategy
 * by writing the results to the standard output.
 * The requests need to be formally correct, in other words – the function
 * is_valid_trip, if called on the same parameters, must return true.
 * 
 * @param   vector of stops to visit in the order of visit
 * @param   vector of routes used to travel between consecutive pairs of stops, in the respective order
 */
void plan_tickets (const std::vector<std::string>& stops, 
                   const std::vector<int>& routes) 
{
    int curr_time = -1;
    int total_time = 0;
    
    std::vector<std::string>::iterator curr_stop = stops.begin();
    std::vector<int>::iterator curr_route = routes.begin();    
    std::vector<std::string>::iterator next_stop = ++stops.begin();

    // zacząłem pisać we wtorek, a potem pomyślałem, że może to nie jest dobry sposób;
    // przemyślę to jeszcze i spróbuję doklepać w miarę możliwości sprawnie
    for(
        curr_route = routes.begin(); curr_route != routes.end(); 
        curr_stop++, curr_route++, next_stop++
    ) {
        if(schedule.count(make_pair(*curr_stop, curr_route) == 0)
        // [TODO]
    }
}

int main(){
    // [TODO] testy i debug
    return 0;
}
