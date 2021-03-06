#include "Player.hh"
#include <list>


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME kiki

// DISCLAIMER: The following Demo player is *not* meant to do anything
// sensible. It is provided just to illustrate how to use the API.
// Please use AINull.cc as a template for your player.

struct PLAYER_NAME : public Player {

    /**
    * Factory: returns a new instance of this class.
    * Do not modify this function.
    */
    static Player* factory () {
        return new PLAYER_NAME;
    }


    /**
    * Types and attributes for your player can be defined here.
    */

    typedef vector<Pos> VP; // Tipus de vector de posicions
    typedef vector<int> VI; // Tipus de vector d'enters
    
    const vector<NutrientType> nutrients = { Carbohydrate, Protein, Lipid };
    vector<VP> table_game;
    vector<VI> dist_food;
    vector<VI> dist_workers_for_soldiers;
    vector<VI> dist_queen;
    VP pos_disp;
    bool leave_food;


    // Default direction to be used during all match.
    Dir default_dir;
    
    bool can_move(Pos p) {
        bool move = true;
        if (cell(p).id != -1) move = false;
        else {
            for (int i = -1; i < 2; ++i) {
                for (int j = -1; j < 2; ++j) {
                    if ((i == -1 and j == 0) or (i == 0 and j == -1) or (i == 0 and j == 1) or (i == 1 and j == 0)) {
                        Pos n = Pos(p.i + i, p.j + j);
                        if (pos_ok(n) and cell(n).id != -1) {
                            Ant a = ant(cell(n).id);
                            if (a.player != me()) move = false;
                        }
                    }
                }
            }
        }
        return move;
    }
    
    bool soldier_enemy_near(Pos p) {
        for (int i = -2; i < 3; ++i) {
            for (int j = -2; j < 3; ++j) {
                Pos n = Pos(p.i + i, p.j + j);
                if (pos_ok(n) and cell(n).id != -1) {
                    Ant a = ant(cell(n).id);
                    if (a.type == Soldier and a.player != me()) return true;
                }
            }
        }
        return false;
    }
    
    bool queen_near(Pos p) {
        for (int i = -4; i < 5; ++i) {
            for (int j = -4; j < 5; ++j) {
                Pos n = Pos(p.i + i, p.j + j);
                if (pos_ok(n) and cell(n).id != -1) {
                    Ant a = ant(cell(n).id);
                    if (a.type == Queen and a.player == me()) return true;
                }
            }
        }
        return false;
    }
    
    int calc_dir_to_escape_soldiers(Pos p) {
        int dir = -1;
        bool ultim = false;
        for (int i = -1; i < 2 and not ultim; ++i) {
            for (int j = -1; j < 2 and not ultim; ++j) {
                Pos n = Pos(p.i + i, p.j + j);
                if (i == -1 and j == 0 and pos_ok(p + Up)) {
                    if (cell(p + Up).id != -1) {
                        Ant a = ant(cell(p + Up).id);
                        if (a.type == Soldier and a.player != me()) {
                            if (cell(p + Left).id == -1 and pos_ok(p + Left)) dir = 3;
                            else if (cell(p + Right).id == -1 and pos_ok(p + Right)) dir = 2;
                            else if (cell(p + Down).id == -1 and pos_ok(p + Down)) dir = 0;
                        }
                    }
                }
                else if (i == 0 and j == -1 and pos_ok(p + Left)) {
                    if (cell(p + Left).id != -1) {
                        Ant a = ant(cell(p + Left).id);
                        if (a.type == Soldier and a.player != me()) {
                            if (cell(p + Up).id == -1 and pos_ok(p + Up)) dir = 1;
                            else if (cell(p + Right).id == -1 and pos_ok(p + Right)) dir = 2;
                            else if (cell(p + Down).id == -1 and pos_ok(p + Down)) dir = 0;
                        }
                    }
                }
                else if (i == 0 and j == -1 and pos_ok(p + Right)) {
                    if (cell(p + Right).id != -1) {
                        Ant a = ant(cell(p + Right).id);
                        if (a.type == Soldier and a.player != me()) {
                            if (cell(p + Up).id == -1 and pos_ok(p + Up)) dir = 1;
                            else if (cell(p + Left).id == -1 and pos_ok(p + Left)) dir = 3;
                            else if (cell(p + Down).id == -1 and pos_ok(p + Down)) dir = 0;
                        }
                    }
                }
                else if (i == 0 and j == -1 and pos_ok(p + Down)) {
                    if (cell(p + Down).id != -1) {
                        Ant a = ant(cell(p + Down).id);
                        if (a.type == Soldier and a.player != me()) {
                            if (cell(p + Up).id == -1 and pos_ok(p + Up)) dir = 1;
                            else if (cell(p + Left).id == -1 and pos_ok(p + Left)) dir = 3;
                            else if (cell(p + Right).id == -1 and pos_ok(p + Right)) dir = 2;
                        }
                    }
                    ultim = true;
                }
            }
        }
        return dir;
    }
    
    int calc_dir_lay(Pos p) {
        int dir = -1;
        bool ultim = false;
        Dir d;
        for (int i = -1; i < 2 and not ultim; ++i) {
            for (int j = -1; j < 2 and not ultim; ++j) {
                if (i == -1 and j == 0 and pos_ok(p + Up) and cell(p + Up).id == -1 and cell(p + Up).type != Water) dir = 2; 
                else if (i == -1 and j == 0 and pos_ok(p + Left) and cell(p + Left).id == -1 and cell(p + Left).type != Water) dir = 3;
                else if (i == -1 and j == 0 and pos_ok(p + Right) and cell(p + Right).id == -1 and cell(p + Right).type != Water) dir = 1;
                else if (i == -1 and j == 0 and pos_ok(p + Down) and cell(p + Down).id == -1 and cell(p + Down).type != Water) {
                    dir = 0;
                    ultim = true;
                }
            }
        }
        return dir;
    }
    
    int calc_dir_queen(Pos p) {
        int dist = 100;
        int dir = -1;
        bool ultim = false;
        for (int i = -1; i < 2 and not ultim; ++i) {
            for (int j = -1; j < 2 and not ultim; ++j) {
                if (i == -1 and j == 0 and pos_ok(p + Up)) {
                    if (dist_food[p.i + i][p.j + j] < dist) {
                        if (cell(p + Up).id != -1) {
                            Ant a = ant(cell(p + Up).id);
                            if (a.player != me() and (a.type == Worker or a.type == Soldier)) {
                                dir = 2; // Dir = Up;
                                dist = dist_food[p.i + i][p.j + j];
                            } 
                        }
                        else {
                            dir = 2; // Dir = Up;
                            dist = dist_food[p.i + i][p.j + j];
                        }
                    }
                }
                else if (i == 0 and j == -1 and pos_ok(p + Left)) {
                    if (dist_food[p.i + i][p.j + j] < dist) {
                        if (cell(p + Left).id != -1) {
                            Ant a = ant(cell(p + Left).id);
                            if (a.player != me() and (a.type == Worker or a.type == Soldier)) {
                                dir = 3; // Dir = Left;
                                dist = dist_food[p.i + i][p.j + j];
                            }
                        }
                        else {
                            dir = 3; // Dir = Left;
                            dist = dist_food[p.i + i][p.j + j];
                        }
                    }
                }
                else if (i == 0 and j == 1 and pos_ok(p + Right)) {
                    if (dist_food[p.i + i][p.j + j] < dist) {
                        if (cell(p + Right).id != -1) { 
                            Ant a = ant(cell(p + Right).id);
                            if (a.player != me() and (a.type == Worker or a.type == Soldier)) {
                                dir = 1; // Dir = Right;
                                dist = dist_food[p.i + i][p.j + j];
                            } 
                        }
                        else {
                            dir = 1; // Dir = Right;
                            dist = dist_food[p.i + i][p.j + j];
                        }
                    }
                }
                else if (i == 1 and j == 0 and pos_ok(p + Down)) {
                    if (dist_food[p.i + i][p.j + j] < dist) {
                        if (cell(p + Down).id != -1) {
                            Ant a = ant(cell(p + Down).id);
                            if (a.player != me() and (a.type == Worker or a.type == Soldier)) {
                                dir = 0; // Dir = Down;
                                dist = dist_food[p.i + i][p.j + j];
                            } 
                        }
                        else {
                            dir = 0; // Dir = Down;
                            dist = dist_food[p.i + i][p.j + j];
                        }
                    }
                    ultim = true;
                }
            }
        }
        return dir;
    }
    
    int calc_dir_kill(Pos p) {
        int dist = 100;
        int dir = -1;
        bool ultim = false;
        for (int i = -1; i < 2 and not ultim; ++i) {
            for (int j = -1; j < 2 and not ultim; ++j) {
                if (i == -1 and j == 0 and pos_ok(p + Up)) {
                    if (dist_workers_for_soldiers[p.i + i][p.j + j] < dist) {
                        if (cell(p + Up).id != -1) {
                            Ant a = ant(cell(p + Up).id);
                            if (a.player != me() and a.type == Worker) {
                                dir = 2; // Dir = Up;
                                dist = dist_workers_for_soldiers[p.i + i][p.j + j];
                            } 
                        }
                        else {
                            dir = 2; // Dir = Up;
                            dist = dist_workers_for_soldiers[p.i + i][p.j + j];
                        }
                    }
                }
                else if (i == 0 and j == -1 and pos_ok(p + Left)) {
                    if (dist_workers_for_soldiers[p.i + i][p.j + j] < dist) {
                        if (cell(p + Left).id != -1) {
                            Ant a = ant(cell(p + Left).id);
                            if (a.player != me() and a.type == Worker) {
                                dir = 3; // Dir = Left;
                                dist = dist_workers_for_soldiers[p.i + i][p.j + j];
                            }
                        }
                        else {
                            dir = 3; // Dir = Left;
                            dist = dist_workers_for_soldiers[p.i + i][p.j + j];
                        }
                    }
                }
                else if (i == 0 and j == 1 and pos_ok(p + Right)) {
                    if (dist_workers_for_soldiers[p.i + i][p.j + j] < dist) {
                        if (cell(p + Right).id != -1) { 
                            Ant a = ant(cell(p + Right).id);
                            if (a.player != me() and a.type == Worker) {
                                dir = 1; // Dir = Right;
                                dist = dist_workers_for_soldiers[p.i + i][p.j + j];
                            } 
                        }
                        else {
                            dir = 1; // Dir = Right;
                            dist = dist_workers_for_soldiers[p.i + i][p.j + j];
                        }
                    }
                }
                else if (i == 1 and j == 0 and pos_ok(p + Down)) {
                    if (dist_workers_for_soldiers[p.i + i][p.j + j] < dist) {
                        if (cell(p + Down).id != -1) {
                            Ant a = ant(cell(p + Down).id);
                            if (a.player != me() and a.type == Worker) {
                                dir = 0; // Dir = Down;
                                dist = dist_workers_for_soldiers[p.i + i][p.j + j];
                            } 
                        }
                        else {
                            dir = 0; // Dir = Down;
                            dist = dist_workers_for_soldiers[p.i + i][p.j + j];
                        }
                    }
                    ultim = true;
                }
            }
        }
        return dir;
    }
    
    int calc_dir_to_exit_queen(Pos p) {
        int dir = -1;
        bool ultim = false;
        int dist = 0;
        for (int i = -1; i < 2 and not ultim; ++i) {
            for (int j = -1; j < 2 and not ultim; ++j) {
                if (i == -1 and j == 0 and pos_ok(p + Up)) {
                    if (dist_queen[p.i + i][p.j + j] >= dist and cell(p + Up).id == -1 and cell(p + Up).type != Water) {
                        dir = 2; // Dir = Up;
                        dist = dist_queen[p.i + i][p.j + j];
                    }
                }
                else if (i == 0 and j == -1 and pos_ok(p + Left)) {
                    if (dist_queen[p.i + i][p.j + j] >= dist and cell(p + Left).id == -1 and cell(p + Left).type != Water) {
                        dir = 3; // Dir = Left;
                        dist = dist_queen[p.i + i][p.j + j];
                    }
                }
                else if (i == 0 and j == 1 and pos_ok(p + Right)) {
                    if (dist_queen[p.i + i][p.j + j] >= dist and cell(p + Right).id == -1 and cell(p + Right).type != Water) {
                        dir = 1; // Dir = Right;
                        dist = dist_queen[p.i + i][p.j + j];
                    }
                }
                else if (i == 1 and j == 0 and pos_ok(p + Down)) {
                    if (dist_queen[p.i + i][p.j + j] >= dist and cell(p + Down).id == -1 and cell(p + Down).type != Water) {
                        dir = 0; // Dir = Down;
                        dist = dist_queen[p.i + i][p.j + j];
                        ultim = true;
                    }
                }
            }
        }
        return dir;
    }
    
    int calc_dir_to_queen(Pos p, int& dist) {
        int dir = -1;
        bool ultim = false;
        int i = 0;
        int j = 0;
        if (dist_queen[p.i][p.j] == 1) dist = 0;
        
        else {
            for (int i = -1; i < 2 and not ultim; ++i) {
                for (int j = -1; j < 2 and not ultim; ++j) {
                    if (i == -1 and j == 0 and pos_ok(p + Up)) {
                        if (dist_queen[p.i + i][p.j + j] < dist and cell(p + Up).id == -1) {
                            dir = 2; // Dir = Up;
                            dist = dist_queen[p.i + i][p.j + j];
                        }
                    }
                    else if (i == 0 and j == -1 and pos_ok(p + Left)) {
                        if (dist_queen[p.i + i][p.j + j] < dist and cell(p + Left).id == -1) {
                            dir = 3; // Dir = Left;
                            dist = dist_queen[p.i + i][p.j + j];
                        }
                    }
                    else if (i == 0 and j == 1 and pos_ok(p + Right)) {
                        if (dist_queen[p.i + i][p.j + j] < dist and cell(p + Right).id == -1) {
                            dir = 1; // Dir = Right;
                            dist = dist_queen[p.i + i][p.j + j];
                        }
                    }
                    else if (i == 1 and j == 0 and pos_ok(p + Down)) {
                        if (dist_queen[p.i + i][p.j + j] < dist and cell(p + Down).id == -1) {
                            dir = 0; // Dir = Down;
                            dist = dist_queen[p.i + i][p.j + j];
                            ultim = true;
                        }
                    }
                }
            }
        }
        return dir;
    }
    
    int calc_dir_food_far_queen(Pos p) {
        int dist = 100;
        int dir = -1;
        bool ultim = false;
        for (int i = -1; i < 2 and not ultim; ++i) {
            for (int j = -1; j < 2 and not ultim; ++j) {
                if (i == -1 and j == 0 and pos_ok(p + Up) and not queen_near(p + Up)) {
                    if (dist_food[p.i + i][p.j + j] < dist and cell(p + Up).id == -1) { //Pot ser que no calgui comprovar si cell(p + Up).id == -1
                        dir = 2; // Dir = Up;
                        dist = dist_food[p.i + i][p.j + j];
                    }
                }
                else if (i == 0 and j == -1 and pos_ok(p + Left) and not queen_near(p + Left)) {
                    if (dist_food[p.i + i][p.j + j] < dist and cell(p + Left).id == -1) {
                        dir = 3; // Dir = Left;
                        dist = dist_food[p.i + i][p.j + j];
                    }
                }
                else if (i == 0 and j == 1 and pos_ok(p + Right) and not queen_near(p + Right)) {
                    if (dist_food[p.i + i][p.j + j] < dist and cell(p + Right).id == -1) {
                        dir = 1; // Dir = Right;
                        dist = dist_food[p.i + i][p.j + j];
                    }
                }
                else if (i == 1 and j == 0 and pos_ok(p + Down) and not queen_near(p + Down)) {
                    if (dist_food[p.i + i][p.j + j] < dist and cell(p + Down).id == -1) {
                        dir = 0; // Dir = Down;
                        dist = dist_food[p.i + i][p.j + j];
                        ultim = true;
                    }
                }
            }
        }
        return dir;
    }
    
    int calc_dir_food(Pos p) {
        int dist = 100;
        int dir = -1;
        bool ultim = false;
        for (int i = -1; i < 2 and not ultim; ++i) {
            for (int j = -1; j < 2 and not ultim; ++j) {
                if (i == -1 and j == 0 and pos_ok(p + Up)) {
                    if (dist_food[p.i + i][p.j + j] < dist and cell(p + Up).id == -1) { //Pot ser que no calgui comprovar si cell(p + Up).id == -1
                        dir = 2; // Dir = Up;
                        dist = dist_food[p.i + i][p.j + j];
                    }
                }
                else if (i == 0 and j == -1 and pos_ok(p + Left)) {
                    if (dist_food[p.i + i][p.j + j] < dist and cell(p + Left).id == -1) {
                        dir = 3; // Dir = Left;
                        dist = dist_food[p.i + i][p.j + j];
                    }
                }
                else if (i == 0 and j == 1 and pos_ok(p + Right)) {
                    if (dist_food[p.i + i][p.j + j] < dist and cell(p + Right).id == -1) {
                        dir = 1; // Dir = Right;
                        dist = dist_food[p.i + i][p.j + j];
                    }
                }
                else if (i == 1 and j == 0 and pos_ok(p + Down)) {
                    if (dist_food[p.i + i][p.j + j] < dist and cell(p + Down).id == -1) {
                        dir = 0; // Dir = Down;
                        dist = dist_food[p.i + i][p.j + j];
                        ultim = true;
                    }
                }
            }
        }
        return dir;
    }
    
    void move_queen() {
        VI my_queen_ids = queens(me());
        int queen_id = my_queen_ids[0];
        Ant queen = ant(queen_id);
        dist_food = vector<VI>(25, VI(25, 100));
        for (int i = 0; i < table_game[0].size(); ++i){
            bfs(table_game[0][i], 0);
        }
        
        int d = -1;
        if (queen.reserve[0] > 0 and queen.reserve[1] > 0 and queen.reserve[2] > 0) {
            d = calc_dir_lay(queen.pos);
            if (d == 0) default_dir = Down;
            else if (d == 1) default_dir = Right;
            else if (d == 2) default_dir = Up;
            else default_dir = Left;
            lay(queen_id, default_dir, Worker);
        }
        else if (round() % 2 == 0) {
            if (d == -1) { 
                d = calc_dir_queen(queen.pos);
                if (d != -1) {
                    if (d == 0) default_dir = Down;
                    else if (d == 1) default_dir = Right;
                    else if (d == 2) default_dir = Up;
                    else default_dir = Left;
                    bool b = true;
                    for (int i = 0; i < pos_disp.size(); ++i) {
                        if (pos_disp[i] == queen.pos + default_dir) b = false;
                    }
                    if (b) {
                        pos_disp.push_back(queen.pos + default_dir);
                        move(queen_id, default_dir);
                    }
                }
            }
        }
    } 
    
    void move_soldiers() {
        VI my_soldier_ids = soldiers(me());
        for(int k = 0; k < my_soldier_ids.size(); ++k) {
            int soldier_id = my_soldier_ids[k];
            Ant soldier = ant(soldier_id);
            int d = calc_dir_kill(soldier.pos);
            if (d != -1) {
                    if (d == 0) default_dir = Down;
                    else if (d == 1) default_dir = Right;
                    else if (d == 2) default_dir = Up;
                    else default_dir = Left;
                    bool b = true;
                    for (int i = 0; i < pos_disp.size(); ++i) {
                        if (pos_disp[i] == soldier.pos + default_dir) b = false;
                    }
                    if (b) {
                        pos_disp.push_back(soldier.pos + default_dir);
                        move(soldier_id, default_dir);
                    }
                
            }
        }
    }
    
    void move_workers() {
        VI my_worker_ids = workers(me());
        VI perm = random_permutation(my_worker_ids.size());
        for (int k = 0; k < int(perm.size()); ++k) {
            int worker_id = my_worker_ids[perm[k]];
            Ant worker = ant(worker_id);
            
            int d;
            if (queen_near(worker.pos)) {
                if (worker.bonus == None) {
                    vector<int> my_queen_ids = queens(me());
                    Ant my_queen = ant(my_queen_ids[0]);
                    bfs(my_queen.pos, 1);
                    d = calc_dir_to_exit_queen(worker.pos);
                    
                    if (d == 0) default_dir = Down;
                    else if (d == 1) default_dir = Right;
                    else if (d == 2) default_dir = Up;
                    else default_dir = Left;
                    if (d != -1) {
                        if (can_move(worker.pos + default_dir)) {
                            bool b = true;
                            for (int i = 0; i < pos_disp.size(); ++i) {
                                if (pos_disp[i] == worker.pos + default_dir) b = false;
                            }
                            if (b) {
                                pos_disp.push_back(worker.pos + default_dir);
                                move(worker_id, default_dir);
                            }
                        }
                    }
                }
                
                else {
                    if (worker.life == 1) {
                        leave(worker_id);
                        leave_food = true;
                    }
                    else {
                        vector<int> my_queen_ids = queens(me());
                        Ant my_queen = ant(my_queen_ids[0]);
                        bfs(my_queen.pos, 1);
                        int dist = 100;
                        d = calc_dir_to_queen(worker.pos, dist);
                        if (dist == 0 and cell(worker.pos).bonus == None) {
                            leave(worker_id);
                            leave_food = true;
                        }
                        else if (dist == 0 and cell(worker.pos).bonus != None) {
                            d = random(0,3);
                            if (d == 0) default_dir = Down;
                            else if (d == 1) default_dir = Right;
                            else if (d == 2) default_dir = Up;
                            else default_dir = Left;
                            if (can_move(worker.pos + default_dir)) {
                                bool b = true;
                                for (int i = 0; i < pos_disp.size(); ++i) {
                                    if (pos_disp[i] == worker.pos + default_dir) b = false;
                                }
                                if (b) {
                                    pos_disp.push_back(worker.pos + default_dir);
                                    move(worker_id, default_dir);
                                }
                            }
                        }
                        else {
                            if (d == 0) default_dir = Down;
                            else if (d == 1) default_dir = Right;
                            else if (d == 2) default_dir = Up;
                            else default_dir = Left;
                            if (d != -1) {
                                if (can_move(worker.pos + default_dir)) {
                                    bool b = true;
                                    for (int i = 0; i < pos_disp.size(); ++i) {
                                        if (pos_disp[i] == worker.pos + default_dir) b = false;
                                    }
                                    if (b) {
                                        pos_disp.push_back(worker.pos + default_dir);
                                        move(worker_id, default_dir);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            
            else {
                if (worker.bonus == None and cell(worker.pos).bonus == None) {
                    d = calc_dir_food_far_queen(worker.pos);
                    if (d != -1) {
                        if (d == 0) default_dir = Down;
                        else if (d == 1) default_dir = Right;
                        else if (d == 2) default_dir = Up;
                        else default_dir = Left;
                        if (can_move(worker.pos + default_dir)) {
                            bool b = true;
                            for (int i = 0; i < pos_disp.size(); ++i) {
                                if (pos_disp[i] == worker.pos + default_dir) b = false;
                            }
                            if (b) {
                                pos_disp.push_back(worker.pos + default_dir);
                                move(worker_id, default_dir);
                            }
                        }
                    }

                }
                
                else if (cell(worker.pos).bonus != None and worker.bonus == None) take(worker_id);
                
                else if (worker.bonus != None) {
                    vector<int> my_queen_ids = queens(me());
                    Ant my_queen = ant(my_queen_ids[0]);
                    bfs(my_queen.pos, 1);
                    int dist = 100;
                    d = calc_dir_to_queen(worker.pos, dist);
                    
                    if (dist == 0) {
                        leave(worker_id);
                        leave_food = true;
                    }
                    else {
                        if (d == 0) default_dir = Down;
                        else if (d == 1) default_dir = Right;
                        else if (d == 2) default_dir = Up;
                        else default_dir = Left;
                        if (d != -1) {
                            if (can_move(worker.pos + default_dir)) {
                                bool b = true;
                                for (int i = 0; i < pos_disp.size(); ++i) {
                                    if (pos_disp[i] == worker.pos + default_dir) b = false;
                                }
                                if (b) {
                                    pos_disp.push_back(worker.pos + default_dir);
                                    move(worker_id, default_dir);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    /*
        0 = bfs_food
        1 = bfs_queen
        2 = bfs_workers_for_soldiers
    */
    void bfs(Pos p, int x) {
        vector<VI> distance(board_rows(), VI(board_cols(),100)); distance[p.i][p.j] = 0;
        vector<vector<bool>> B(25, vector<bool>(25,false));
        queue<Pos> Q;
        Q.push(p);
        while (not Q.empty()) {
            Pos m = Q.front(); Q.pop();
            if (not B[m.i][m.j]) { 
                B[m.i][m.j] = true;
                for (int i = -1; i < 2; ++i) {
                    for (int j = -1; j < 2; ++j) {
                        if ((i == -1 and j == 0) or (i == 0 and j == -1) or (i == 0 and j == 1) or (i == 1 and j == 0)) {
                            Pos n = Pos(m.i + i, m.j + j);
                            if (pos_ok(n) and cell(n).type != Water){
                                if (distance[n.i][n.j] > distance[m.i][m.j] + 1) {
                                    distance[n.i][n.j] = distance[m.i][m.j] + 1;
                                    Q.push(n);
                                }
                            }
                        }
                    }
                }
            }
        }
        
        if (x == 0) // bfs_food
        for (int i = 0; i < 25; ++i){
            for (int j = 0; j < 25; ++j){
                dist_food[i][j] = min(dist_food[i][j], distance[i][j]);
            }
        }
        
        else if (x == 1) { //bfs_queen
            for (int i = 0; i < 25; ++i){
                for (int j = 0; j < 25; ++j){
                    dist_queen[i][j] = min(dist_queen[i][j], distance[i][j]);
                }
            }
        }
        
        else if (x == 2) { //bfs_workers_for_soldiers
            for (int i = 0; i < 25; ++i){
                for (int j = 0; j < 25; ++j){
                    dist_workers_for_soldiers[i][j] = min(dist_workers_for_soldiers[i][j], distance[i][j]);
                }
            }
        }
    }
    
    void read_table_game(){
        if (table_game.size() > 0) {
            table_game[0].erase(table_game[0].begin(), table_game[0].end());
            table_game[1].erase(table_game[1].begin(), table_game[1].end());
        }
        table_game = vector<VP> (2);
        Pos p;
        for (int i = 0; i < board_rows(); ++i){
            for (int j = 0; j < board_cols(); ++j){
                p = {i,j};
                Cell c = cell(p);
                if (c.bonus != None) table_game[0].push_back(p);
                if (c.id != -1) {
                    Ant a = ant(c.id);
                    if (a.player != me() and a.type == Worker) table_game[1].push_back(p);
                }
            }
        }
    }

    /**
    * Play method, invoked once per each round.
    */
    virtual void play () {
        
        double st = status(me());
        if (st >= 0.9) return;
        
        read_table_game();
        
        if (round() == 0) {
            VI my_worker_ids = workers(me());
            VI perm = random_permutation(my_worker_ids.size());
            default_dir = Up;
            if (me() == 0 or me() == 1) default_dir = Down;
            for (int k = 0; k < int(perm.size()); ++k) {
                int worker_id = my_worker_ids[perm[k]];
                Ant worker = ant(worker_id);
                Pos p = worker.pos + default_dir;
                Cell c = cell(p);
                if (c.type != Water and c.id == -1)
                    move(worker_id, default_dir); 
            }
            leave_food = false;
        }
        
        else if (round() > 0) {
            dist_workers_for_soldiers = vector<VI>(25, VI(25, 100));
            vector<int> my_soldier_ids = soldiers(me());
            for (int i = 0; i < table_game[1].size(); ++i){
                bfs(table_game[1][i], 2);
            }
        }
        
        // Llegir la matriu quan surten les llavors
        if ((round() % 25) == 1) { 
            dist_food = vector<VI>(25, VI(25, 100));
            for (int i = 0; i < table_game[0].size(); ++i){
                bfs(table_game[0][i], 0);
            }
            
            dist_queen = vector<VI>(25, VI(25, 100));
            vector<int> my_queen_ids = queens(me());
            Ant my_queen = ant(my_queen_ids[0]);
        }
        
        if (leave_food) {
            leave_food = false;
            dist_food = vector<VI>(25, VI(25, 100));
            for (int i = 0; i < table_game[0].size(); ++i){
                bfs(table_game[0][i], 0);
            }
        } 
        
        
        /* Extres:
            Fer una comprovacio de la vida de les treballadores i quan queda poc deixar el menjar.
        */
        
        if (round()%2 == 0 and round() > 0) {
            //Cal fer un read table game for queen que sigui nom????s al seu voltant
            dist_food = vector<VI>(25, VI(25, 100));
            for (int i = 0; i < table_game[0].size(); ++i){
                bfs(table_game[0][i], 0);
            }
        }
        
        if (round() > 0) {
            move_queen();
            move_workers();
            move_soldiers();
            
            while(pos_disp.size() > 0) pos_disp.pop_back();
        }
    }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
