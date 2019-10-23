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
    pos=Pos.new(-1,-1)
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

def Calc(list,flag)
	val=list[0]
	for i in 1..list.size-1
			if flag and val<list[i]
				val=list[i]
			elsif not flag and val>list[i]
				val=list[i]
			end
	end
	return val
end

def MonteCarlo(board,turn,side,width,turnNum)#MonteCarloAlgorithm
	if turnNum==side**2-4
			return 0
	end
	valList=[]
	posList=[]
	side.times do|i|
			side.times do|j|
				cpboard=Marshal.load(Marshal.dump(board))
				if Put?(Pos.new(j,i),cpboard,turn,side)
						posList.push(Pos.new(j,i))
				end
			end
	end
	width=posList.size if posList.size<width
	width.times do|i|
		cpboard=Marshal.load(Marshal.dump(board))
		index=Random.new.rand(0..posList.size-1)
		Put?(posList[index],cpboard,turn,side)
		cpboard[posList[index].y][posList[index].x]=turn
		valList.push(MonteCarlo(cpboard,turn*-1,side,width,turnNum+1))
	end
	return MonteCarlo(board,turn*-1,side,width,turnNum+1) if width.zero?
	if $myturn==turn
		if turnNum.zero?
				index=valList.index(Calc(valList,true))
				$bestPos=Pos.new(posList[index].x,posList[index].y)
				return 0
		end
		return Calc(valList,true)
	else
		if turnNum.zero?
				index=valList.index(Calc(valList,false))
				$bestPos=Pos.new(posList[index].x,posList[index].y)
				return 0
		end
		return Calc(valList,false)
	end
end

def AlphaBeta()#AlphaBetaAlgorithm
	
end
