#include <iostream>
#include <algorithm>
#include <vector>
#include <limits>
#include <cmath>

// In the task the longest trip can take 927 minutes. 
const int MAX_TRIP_LENGTH = 927;

// For the purpose of 'add_new_ticket' and 'get_optimal_ticket_set'
// function it is easier to take a small margin(at least 2) so that
// we don't have to worry about exceeding the vector size.
const int TICKET_SET_SIZE = 1000; 

// A vector containing ticket information(pair<name, expiration_time>)
std::vector<std::pair<std::string, int> > tickets;

// An array of vectors. Each of the vectors represents best possible
// price for a given time(vector[i][k] = pair<best price for i + 1 tickets
// and the time equals to k, id of the latest used ticket to produce the price>).
std::vector<std::pair<int, int> > optimal_ticket_set[3];

/**
 *  Initializes ressources for 'add_new_ticket' and 'get_optimal_ticket_set'
 *  functions. 
 */
void initialize_optimal_ticket_set() {

    std::pair<int, int> default_pair = std::make_pair(INT_MAX, -1);

    for (int i = 0; i < 3; i++) {
        optimal_ticket_set[i].resize(TICKET_SET_SIZE, default_pair);
        optimal_ticket_set[i][0] = std::make_pair(0, -1);
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
void add_new_ticket(const std::string &ticket_name, const int price, int expiration_time) {

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
std::vector<std::string> get_optimal_ticket_set(const int trip_length) {

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

//Tests:

void print_vector(std::vector<std::string> vec) {
    std::cout << "Array: ";

    for (size_t i = 0; i < vec.size()-1; i++)
        std::cout << vec[i] << ", ";

    if (vec.size() != 0)
        std::cout << vec[vec.size() - 1];
    std::cout << "\n";
}

int main() {

    initialize_optimal_ticket_set();

    add_new_ticket("Ticket 1", 100, 10);
    add_new_ticket("Ticket 2", 1100, 100);
    add_new_ticket("Ticket 3", 205, 20);

    print_vector(get_optimal_ticket_set(1));
    print_vector(get_optimal_ticket_set(10));
    print_vector(get_optimal_ticket_set(20));
    print_vector(get_optimal_ticket_set(30));
    print_vector(get_optimal_ticket_set(40));
    print_vector(get_optimal_ticket_set(110));
    print_vector(get_optimal_ticket_set(900));

    return 0;
}
