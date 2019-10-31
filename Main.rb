require "./Algorithm"
require "./Handy"
require "socket"

def Main()#MainFunction
	side=10#SideLength
	puts "Please input your turn" 
    #Send message of "Recruit" to server
    #p $myturn=Socket("Recruit")
    inputTurn=gets.chomp
    case inputTurn 
    when "black" then
      $myturn=1
    when "white" then
      $myturn=-1
    end
	board=Array.new(side).map{Array.new(side){0}}
	turn=1
	board[side/2-1][side/2-1],board[side/2][side/2]=1,1#0:None 1:black -1:white
	board[side/2-1][side/2],board[side/2][side/2-1]=-1,-1
	counter=0
	Print(board)
    boardInfo=0 #0:first 1:midium 2:final
    pathcnt=0
    while true
      break if pathcnt>=2
        if counter.between?(side**2/3,side**2/3*2)
          boardInfo=1
        elsif counter.between?(side**2/3*2,side**2-1)
          boardInfo=2
        end
        white,black=CheckPieces(board,side)
        puts "White:#{white} Black:#{black}"
		if turn!=$myturn
			input=gets.chomp.split(",")#input[0]=x input[1]=y
          if $bestPos==nil
            $bestPos=Pos.new(-1,-1)
          end
          #input=Socket("turn:#{$myturn}:#{$bestPos.x},#{$bestPos.y}")
		else
			$level=4
            if FullMap?(board,side).zero?
                break
            elsif $level>=side**2-4-counter
			  $level=side**2-4-counter
            end
			#MinMax(Marshal.load(Marshal.dump(board)),turn,side,$level,boardInfo)
			#MonteCarlo(Marshal.load(Marshal.dump(board)),turn,side,2,0)
			AlphaBeta(Marshal.load(Marshal.dump(board)),turn,side,$level,boardInfo)
			input=[]
			if($bestPos.x==-1 and $bestPos.y==-1)
					puts "Path"
					turn*=-1
                    pathcnt+=1
					next
            else
              pathcnt=0
			end
			input[0],input[1]=$bestPos.x,$bestPos.y
            p input
  		end
		break if input[0]=="stop"
		if input[0]=="path"
				turn*=-1
                pathcnt+=1
				next
        else
          pathcnt=0
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
	  end 
end

def Put?(pos,putboard,turn,side)#Whether to can put piece or not
	dir_x=[1,1,0,-1,-1,-1,0,1]#Next straight direction
	dir_y=[0,-1,-1,-1,0,1,1,1]
	flag=false
	8.times do|i|#Eight's direction search
      if (pos.x+dir_x[i]).between?(0,side-1) and (pos.y+dir_y[i]).between?(0,side-1) and putboard[pos.y+dir_y[i]][pos.x+dir_x[i]]==turn*-1 and putboard[pos.y][pos.x].zero? #Judge whether to can put piece or not
				judge=Judge(pos,putboard,turn,side,dir_x[i],dir_y[i])
				flag=judge if not flag
			end
	end
	return flag
end

def Judge(pos,judgeboard,turn,side,dirX,dirY)#Actually confirm whether to put piece or not
	if not (pos.x+dirX).between?(0,side-1) or not (pos.y+dirY).between?(0,side-1) or judgeboard[pos.y+dirY][pos.x+dirX].zero? #Don't get caught
		return false
	elsif judgeboard[pos.y+dirY][pos.x+dirX]==turn#Pieces of enemy gets caught by mine
		return true
	else#Not been determine
		nextpos=Pos.new(pos.x+dirX,pos.y+dirY)
		if Judge(nextpos,judgeboard,turn,side,dirX,dirY)
			judgeboard[nextpos.y][nextpos.x]=turn
			return true
		else
			return false end
	end
end

def Print(board)#Print the current board
  print " "
  (board.size).times do|i|
    print " #{i}"
  end
    print "\n"
		(board.size).times do|i|
          print"#{i}"
				(board.size).times do|j|
						case board[i][j]
						when 0 then
								print " *"
						when 1 then
								print " ○"
						when -1 then
								print " ●" 
						end
				end
				print "\n"
		end
end
def CheckPieces(board,side)
    white=0
    black=0
    side.times do|i|
      side.times do|j|
        case board[i][j]
        when 1 then
          black+=1
        when -1 then
          white+=1
        end
      end
    end 
    return white,black
end 

def Socket(sendMessage)
    ipAddress=Socket.getifaddrs.select{|x|
      x.name=="en0" and x.addr.ipv4?
    }.first.addr.ip_address
    connect=TCPSocket.open(ipAddress,50000)
    connect.print sendMessage
    #receivePos=connect.gets.chomp.split(",").map(&:to_i)
    neccesallyInfo=nil
    receiveMessage=connect.recv(100).chomp.split(":")
    puts receiveMessage
    case receiveMessage[0]
    when "" then
    else
        neccesallyInfo=receiveMessage[0].to_i
    end
    connect.close
    return neccesallyInfo
end
Main()
