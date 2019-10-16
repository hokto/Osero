require "./Algorithm"
require "./Handy"

def Main()#MainFunction
	side=10#SideLength
	board=Array.new(side).map{Array.new(side){0}}
	turn=1
	board[side/2-1][side/2-1],board[side/2][side/2]=1,1#0:None 1:black -1:white
	board[side/2-1][side/2],board[side/2][side/2-1]=-1,-1
	counter=0
	Print(board)
	begin
		#if turn==1
			#input=gets.chomp.split(",")#input[0]=y input[1]=x
		#else
			$level=3
			$level=97-counter if $level>=97-counter
			MinMax(Marshal.load(Marshal.dump(board)),turn,side,$level)
			input=[]
			if($bestPos.x==-1 and $bestPos.y==-1)
					puts "Path"
					turn*=-1
					next
			end
			input[0],input[1]=$bestPos.x,$bestPos.y
		#end
		break if input[0]=="stop"
		if input[0]=="path"
				turn*=-1
				next
		end
		pos=Pos.new(input[0].to_i,input[1].to_i)
		if Put?(pos.dup,board,turn,side)
				board[pos.y][pos.x]=turn
		else
				puts "One more!"
				next
		end	
		turn*=-1#Turn switching
		counter+=1
		Print(board)
		print "\n"
		end while counter<side**2-4	
end

def Put?(pos,putboard,turn,side)#Whether to can put piece or not
	dir_x=[1,1,0,-1,-1,-1,0,1]#Next straight direction
	dir_y=[0,-1,-1,-1,0,1,1,1]
	flag=false
	8.times do|i|#Eight's direction search
			if (pos.x+dir_x[i]).between?(0,side-1) and (pos.y+dir_y[i]).between?(0,side-1) and putboard[pos.y+dir_y[i]][pos.x+dir_x[i]]==turn*-1 and putboard[pos.y][pos.x]==0 #Judge whether to can put piece or not
				judge=Judge(pos,putboard,turn,side,dir_x[i],dir_y[i])
				flag=judge if !flag
			end
	end
	return flag
end

def Judge(pos,judgeboard,turn,side,dirX,dirY)#Actually confirm whether to put piece or not
		if not (pos.x+dirX).between?(0,side-1) or not (pos.y+dirY).between?(0,side-1)or judgeboard[pos.y+dirY][pos.x+dirX].zero? #Don't get caught
		return false
	elsif judgeboard[pos.y+dirY][pos.x+dirX]==turn#Pieces of enemy gets caught by mine
		return true
	else#Not been determine
		nextpos=Pos.new(pos.x+dirX,pos.y+dirY)
		if Judge(nextpos,judgeboard,turn,side,dirX,dirY)
			judgeboard[nextpos.y][nextpos.x]=turn
			return true
		else
			return false
		end
	end
end
def Print(board)#Print the current board
		(board.size).times do|i|
				(board.size).times do|j|
						case board[i][j]
						when 0 then
								print "□"
						when 1 then
								print "○"
						when -1 then
								print "●" 
						end
				end
				print "\n"
		end
end
Main()