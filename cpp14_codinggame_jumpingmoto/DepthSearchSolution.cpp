#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <stack>

using namespace std;

struct state{
    int pos;
    int speed;

	bool operator== (const state& oth){
		return pos==oth.pos && speed == oth.speed;
	}
	
	bool operator!= (const state& oth){
		return !( *this == oth );
	}
};

enum class action {faster, slower, jump, wait };
enum class state_kind {goal, valid, death};

vector<action> searchSolution(state s0, int road, int gap, int plat){
   	using action_state = pair<action, state>; 
    	using path = vector<action_state>;

	auto apply = [](action_state as) -> state {
		state s=get<1>(as);
		switch(get<0>(as)){
			case action::faster:
				++s.speed;
			break;
			case action::slower:
				--s.speed;
			break;
		}
		s.pos += s.speed;
		return s;
	};


	auto analyze = [=](state s, action_state prev) -> state_kind {
		if(s == get<1>(prev))
			return state_kind::death;
		if(s.speed < 0)
			return state_kind::death;
		if(s.pos>=0 && s.pos < road)
			return state_kind::valid;
		if(s.pos>=road && s.pos < road+gap)
			return state_kind::death;
		if(s.pos>= road+gap && s.pos < road+gap+plat){
			if(get<1>(prev).pos < road && get<0>(prev) != action::jump){
				return state_kind::death;
			}else{
				return s.speed==0? state_kind::goal: state_kind::valid;
			}
		}
		if(s.pos>= road+gap+plat)
			return state_kind::death;

	};

	auto extractPath = [](path p) -> vector<action> {
		vector<action> res;
		for(auto as: p){
			res.push_back(get<0>(as));
		}
		return res;
	};
    	stack<path> search_space({	{{action::jump, s0}},
                	        	{{action::wait, s0}},
        	                	{{action::slower, s0}},
                        		{{action::faster, s0}}
			     	});

	while(!search_space.empty()){
		path p= search_space.top();
		search_space.pop();

		state res = apply(p.back());
		
		switch(analyze(res, p.back())){
		case state_kind::valid:
			search_space.emplace(p);
			search_space.top().push_back(make_pair(action::jump, res));
			search_space.emplace(p);
			search_space.top().push_back(make_pair(action::wait, res));
			search_space.emplace(p);
			search_space.top().push_back(make_pair(action::slower, res));
			search_space.emplace(p);
			search_space.top().push_back(make_pair(action::faster, res));
		break;
		case state_kind::goal:
			return extractPath(p);
		break;
		}
	}
	
	//no solution
	throw "no solution :(";

}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    int road; // the length of the road before the gap.
    cin >> road;
    int gap; // the length of the gap.
    cin >> gap;
    int platform; // the length of the landing platform.
    cin >> platform;

	
	int speed;
	cin >> speed;
	int pos;
	cin >> pos;
	
	auto seq = searchSolution({pos, speed}, road, gap, platform);
	
	auto a_it=begin(seq);
	
	while(1){
	auto a = *a_it;
	++a_it;
		switch(a){
		case action::faster:
			cout << "SPEED";
		break;
		case action::slower:
			cout << "SLOW";
		break;
		case action::jump:
			cout << "JUMP";
		break;
		case action::wait:
			cout << "WAIT";
		break;
		}
		cout << '\n';
	int speed;
	cin >> speed;
	int pos;
	cin >> pos;
	}
}
