void read_table_game(){
        table_game = vector<VP> (5);
        Pos p;
        for (int i = 0; i < board_rows(); ++i){
            for (int j = 0; j < board_cols(); ++j){
                p = {i,j};
                Cell c = cell(p);
                if (c.type == Soil) {
                    table_game[0].push_back(p);
                    if (c.bonus == Bread) table_game[2].push_back(p);
                    else if (c.bonus == Leaf) table_game[3].push_back(p);
                    else if (c.bonus == Seed) table_game[4].push_back(p);
                }
                else table_game[1].push_back(p);
            }
        }
    }
    
    
    if (soldier_queen_enemy_near(worker.pos)) {
                cout << "Ara sí, ant: " << worker_id << endl;
                if (can_move(worker.pos + Up)) {
                    cout << "Amunt" << endl;
                    bool b = true;
                    for (int i = 0; i < pos_disp.size(); ++i) {
                        if (pos_disp[i] == worker.pos + Up) b = false;
                    }
                    default_dir = Up;
                    pos_disp.push_back(worker.pos + default_dir);
                    move(worker_id, default_dir);
                }
                if (can_move(worker.pos + Down)) {
                    cout << "Avall" << endl;
                    bool b = true;
                    for (int i = 0; i < pos_disp.size(); ++i) {
                        if (pos_disp[i] == worker.pos + Down) b = false;
                    }
                    if (soldier_queen_enemy_near(worker.pos + Down)) b = false;
                    cout << b << endl;
                    if (b) {
                        default_dir = Down;
                        pos_disp.push_back(worker.pos + default_dir);
                        move(worker_id, default_dir);
                    }
                }
                if (can_move(worker.pos + Left)) {
                    cout << "Esquerra" << endl;
                    bool b = true;
                    for (int i = 0; i < pos_disp.size(); ++i) {
                        if (pos_disp[i] == worker.pos + Left) b = false;
                    }
                    if (soldier_queen_enemy_near(worker.pos + Left)) b = false;
                    if (b) {
                        default_dir = Left;
                        pos_disp.push_back(worker.pos + default_dir);
                        move(worker_id, default_dir);
                    }
                }
                if (can_move(worker.pos + Right)) {
                    cout << "Dreta" << endl;
                    bool b = true;
                    for (int i = 0; i < pos_disp.size(); ++i) {
                        if (pos_disp[i] == worker.pos + Right) b = false;
                    }
                    if (soldier_queen_enemy_near(worker.pos + Right)) b = false;
                    if (b) {
                        default_dir = Right;
                        pos_disp.push_back(worker.pos + default_dir);
                        move(worker_id, default_dir);
                    }
                }
            }
