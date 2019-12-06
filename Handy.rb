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
        elsif boardInfo==2 and Put?(Pos.new(j,i),Marshal.load(Marshal.dump(board)),turn,side)
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

def CanPut(board,turn,side)
  posList=[]
  side.times do|i|
    side.times do|j|
      if Put?(Pos.new(j,i),Marshal.load(Marshal.dump(board)),turn,side)
        posList.push(Pos.new(j,i))
      end
    end
  end
  return posList
end

def SortNode(board,evaboard,turn,side,boardInfo,posList,abflag)
  evaList=[]
  posList.each do|pos|
    cpboard=Marshal.load(Marshal.dump(board))
    Put?(pos,cpboard,turn,side)
    cpboard[pos.y][pos.x]=turn
    evaList.push(CarcuEva(cpboard,evaboard,turn,side,boardInfo))
  end  
  return QuickSortOfNode(posList,evaList,0,posList.size-1,abflag)
end

def QuickSortOfNode(targetList,sortList,left,right,abflag)
  i=left+1
  k=right
  while i<k
    while (sortList[i]>sortList[left] and abflag==0) or (sortList[i]<sortList[left] and abflag==1)and i<right
      i+=1
    end
    while (sortList[k]<=sortList[left] and abflag==0) or (sortList[k]>=sortList[left] and abflag==1) and k>left
      k-=1
    end
    if i<k
      sortList[i],sortList[k]=sortList[k],sortList[i]
      targetList[i],targetList[k]=targetList[k],targetList[i]
    end
  end
  if(sortList[left]<sortList[k] and abflag==0) or (sortList[left]>sortList[k] and abflag==1)
    sortList[left],sortList[k]=sortList[k],sortList[left]
    targetList[left],targetList[k]=targetList[k],targetList[left]
  end
  QuickSortOfNode(targetList,sortList,left,k-1,abflag) if left<k-1
  QuickSortOfNode(targetList,sortList,k+1,right,abflag) if k+1<right
  return targetList
end
