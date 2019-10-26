class Pos #Position class
	attr_accessor:x,:y
	def initialize(x,y)
		@x,@y=x,y
	end
end

def CarcuEva(board,evaboard,turn,side,boardInfo)#Carcuation evacuation of current board
	value=0#My evacuation 
    canPutVal=0#My prediction value
    enemyValue=0#Enemy evacuation
    enemyCanPutVal=0#Enemy's prediction value
	side.times do|i|
			side.times do|j|
					if board[i][j]==turn
							dir_x=[1,1,0,-1,-1,-1,0,1]
							dir_y=[0,-1,-1,-1,0,1,1,1]
							value+=evaboard[i][j]
							nonecnt=0#side's pieces
							8.times do|k|
									if not (i+dir_y[k]).between?(0,side-1) or not (j+dir_x[k]).between?(0,side-1)
										nonecnt+=1
									end
							end
							value+=evaboard[i][j]*(nonecnt-1) if not nonecnt.zero?#Value increase if the more I have side's pieces
                    elsif board[i][j]==turn*-1
                            enemyValue+=evaboard[i][j]
                    else
                            if Put?(Pos.new(j,i),Marshal.load(Marshal.dump(board)),turn*-1,side)#Whether to enemy can put
                                  if boardInfo!=2
                                    enemyCanPutVal+=evaboard[i][j]/2
                                  else
                                    enemyCanPutVal+=evaboard[i][j]
                                  end
                            elsif boardInfo==2 and Put?(Pos.new(j,i),Marshal.load(Marshal.dumo(board)),turn,side)
                                  canPutVal+=evaboard[i][j]/2
                            end
					end
			end
	end
	return value+canPutVal-enemyValue-enemyCanPutVal
end

def FullMap?(board,side)#Whether to the board is full
    count=0
    side.times do|i|
      side.times do|j|
        if board[i][j].zero?
          count+=1
        end
      end
    end 
   return count
end
