require "./Handy"

$bestPos=nil
def MinMax(board,turn,side,depth)#MinMaxAlgorithm
	$bestPos=Pos.new(-1,-1)
	evaboard=[]
	File.open("./Evaboard.txt",mode="rt"){
			|file|
			file.each_line{
				|line|
					evaboard.push(line.split(",").map(&:to_i))
			}
	}
	return MaxAlgo(board,evaboard,turn,side,depth)
end

def MaxAlgo(board,evaboard,turn,side,depth)
	return CarcuEva(board,evaboard,turn,side) if depth.zero?
	max=-10**9
	pos=Pos.new(-1,-1)
	side.times do|i|
			side.times do|j|
					cpboard=Marshal.load(Marshal.dump(board))
					if Put?(Pos.new(j,i),cpboard,turn,side)
							cpboard[i][j]=turn
							score=MinAlgo(cpboard,evaboard,turn*-1,side,depth-1)
							if score>max
								max=score
								pos=Pos.new(j,i)
							end
					end
			end
	end
	$bestPos=pos if depth==$level
	return max
end

def MinAlgo(board,evaboard,turn,side,depth)
	return CarcuEva(board,evaboard,turn,side) if depth.zero?
	min=10**9
	side.times do|i|
			side.times do|j|
					cpboard=Marshal.load(Marshal.dump(board))
					if Put?(Pos.new(j,i),cpboard,turn,side)
							cpboard[i][j]=turn
							score=MaxAlgo(cpboard,evaboard,turn*-1,side,depth-1)
							if score<min
									min=score
							end
					end
			end
	end
	return min
end

def MonteCarlo()#MonteCarloAlgorithm

end

def AlphaBeta()#AlphaBetaAlgorithm

end
