class Pos
	attr_accessor:x,:y
	def initialize(x,y)
		@x,@y=x,y
	end
end
def CarcuEva(board,evaboard,turn,side)
	value=0
	side.times do|i|
			side.times do|j|
					if board[i][j]==turn
							dir_x=[1,1,0,-1,-1,-1,0,1]
							dir_y=[0,-1,-1,-1,0,1,1,1]
							value+=evaboard[i][j]
							nonecnt=0
							8.times do|k|
									if not (i+dir_y[k]).between?(0,side-1) or not (j+dir_x[k]).between?(0,side-1)
										nonecnt+=1
									end
							end
							value+=evaboard[i][j]*(nonecnt-1) if not nonecnt.zero?
					end
			end
	end
	return value
end
