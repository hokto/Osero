use std::fs::File;
use std::io::{BufRead,BufReader};
struct Pos{
    x: i8,
    y: i8,
}

fn input() ->String{ //Input Function
    let mut input=String::new();
    std::io::stdin().read_line(&mut input).ok();
    input=input.replace("\n","");
    return input;
}
fn print_board(SIDE:usize ,board: &Vec<Vec<i8>>){ //Board Print
    print!(" ");
    for i in 0..SIDE{
        print!(" {}",i);
    }
    print!("\n");
    for i in 0..SIDE{
        print!("{} ",i);
        for j in 0..SIDE{
            match board[i][j]{
                1=>print!("● "),
                -1=>print!("○ "),
                0=>print!("□ "),
                _=>print!(""),
            }
        }
        print!("\n");
    }
} 
fn read_file(filename: String,SIDE:usize) -> Vec<Vec<i8>>{ //ファイルを読み込んで二次元配列で返す
    let file=File::open(filename).unwrap();
    let reader=BufReader::new(file);
    let mut eva_board: Vec<Vec<i8>>=vec![vec![0;SIDE];SIDE];
    for (i,line) in reader.lines().enumerate(){
        eva_board[i]=line.unwrap().split(",").map(|s| s.parse().unwrap()).collect();
    }
    return eva_board
}
fn between(val: &i8,min_val: &i8,max_val:&i8) -> bool{ //valがmin_val,max_valの間にあるかどうか
    if min_val<=val&&val<=max_val{
        return true;
    }else{
        return false;
    }
}

fn between_pos(check_pos: &Pos,min_pos: &Pos,max_pos: &Pos) -> bool{
    if between(&check_pos.x as &i8,&min_pos.x as &i8,&max_pos.x as &i8)&&between(&check_pos.y as &i8,&min_pos.y as &i8,&max_pos.y as &i8){
        return true;
    }else{
        return false;
    }
}

fn board_judge(current_pos:&Pos,board:&mut Vec<Vec<i8>>,turn:&i8,SIDE:usize,dx:i8,dy:i8) -> bool{
    if !between_pos(&current_pos,&Pos{x:0,y:0},&Pos{x:(SIDE-1) as i8,y:(SIDE-1) as i8})||board[(current_pos.y) as usize][(current_pos.x) as usize]==0{
        return false;
    }
    else if board[current_pos.y as usize][current_pos.x as usize]==*turn{
        return true;
    }
    else{
        let next_pos=Pos{x:current_pos.x+dx,y:current_pos.y+dy};
        if board_judge(&next_pos,board,&turn,SIDE,dx,dy){
            board[current_pos.y as usize][current_pos.x as usize]=*turn;
            return true;
        }
        else{
            return false;
        }
    }
}
fn board_put(select_pos: &Pos,board: &mut Vec<Vec<i8>>,turn:&i8,SIDE:usize)->bool{
    let dir_x=[1,1,0,-1,-1,-1,0,1];
    let dir_y=[0,-1,-1,-1,0,1,1,1];
    let mut flag:bool=false;
    for i in 0..7{
        if between_pos(&select_pos,&Pos{x:0,y:0},&Pos{x:(SIDE-1) as i8,y:(SIDE-1) as i8})&&board[(select_pos.y+dir_y[i]) as usize][(select_pos.x+dir_x[i]) as usize]==turn*-1&&board[select_pos.y as usize][select_pos.x as usize]==0{
          let current_pos=Pos{x:select_pos.x+dir_x[i],y:select_pos.y+dir_y[i]};
          let judge:bool=board_judge(&current_pos,board,&turn,SIDE,dir_x[i],dir_y[i]);
          if !flag{
              flag=judge;
          }
        }
    }
    return flag;
}

fn Alpha(board:&mut Vec<Vec<i8>>,eva_board:Vec<Vec<i8>>,turn:&mut i8,SIDE: usize,depth: &mut i8,board_info:&i8,alpha:i32,beta:i32){
        
}
fn AlphaBeta(best_pos:&Pos,board:&mut Vec<Vec<i8>>,turn:&i8,SIDE:usize,depth:i8,board_info:&i8)//アルファベータ法
{
    let eva_board:Vec<Vec<i8>>=read_file("../EvaBoard.txt".to_string(),SIDE);
    return Alpha(&mut board,eva_board,&mut turn,SIDE,&mut depth,&board_info,-100_000_000,100_000_000);
}
fn main(){ //Main Function
    const SIDE:usize=10;
    println!("Please input your turn");
    let mut input_turn=String::new();
    input_turn=input();
    let mut my_turn=0;
    let mut turn=-1;//黒からはじめ
    let mut best_pos=Pos{x:0,y:0};
    if input_turn=="white"{
        my_turn=1;
    }
    else if input_turn=="black"{
        my_turn=-1;
    }
    let mut board: Vec<Vec<i8>>=vec![vec![0;SIDE];SIDE];
    board[SIDE/2-1][SIDE/2-1]=-1;
    board[SIDE/2][SIDE/2]=-1;
    board[SIDE/2-1][SIDE/2]=1;
    board[SIDE/2][SIDE/2-1]=1;
    println!("Turn:{}",my_turn); 
    let mut path_cnt:i8=0;
    let mut counter:i8 =0;
    let mut board_info:i8=0;
    AlphaBeta(&best_pos,&mut board,&turn,SIDE,5,&board_info);
    loop{
        print_board(SIDE,&board);
        let mut path_flag:bool=false;
        if path_cnt>=2{
            break
        }
       if between(&counter,&((SIDE as i8).pow(2/3)),&(SIDE as i8).pow(2/3*2)){
           board_info=1;
       }
       else if between(&counter,&(SIDE as i8).pow(2/3*2),&((SIDE as i8).pow(2)-1)){
           board_info=2;
       }
       let mut current_pos=Pos{x:0,y:0};
       //if turn != my_turn{
           let input=input();
           if input=="path"{
                path_flag=true;
           }
           if path_flag{
               turn*=-1;
               path_cnt+=1;
               if path_cnt>=2{
                   break;
               }
               continue;
           }
           else{
               path_cnt=0;
           }
           let input_pos:Vec<&str>=input.split(",").collect();
           let mut select_pos=Pos{x:0,y:0};
           select_pos.x=input_pos[0].parse().unwrap();
           select_pos.y=input_pos[1].parse().unwrap();
           current_pos=select_pos;
           //}
           /*else{
             }*/
           if board_put(&current_pos,&mut board,&turn,SIDE){
               board[current_pos.y as usize][current_pos.x as usize]=turn;
           }
           else{
               println!("One more!");
               continue;
           }
           turn*=-1;
           counter+=1;
           print_board(SIDE,&board);
           print!("\n");
    }
} 
