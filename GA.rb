class Gene
  attr_accessor:boardEva,:point
  @@count=0
  def initialize(number)
    @number=number
    @boardEva=Array.new(4)
    File.open("./Generic/gene#{@number}.txt") do|file|
      i=0
      file.each_line(rs="\n") do|line|
        @boardEva[i]=line.chomp.split(" ").map(&:to_i) 
        i+=1
      end 
    end
    @point=@@count
    @@count+=1
  end 
end

def GeneSort(geneList,left,right)
  i=left+1
  k=right
  while i<k
    i+=1 while geneList[i].point<=geneList[left].point and i>right
    k-=1 while geneList[k].point>geneList[left].point and k>left
    geneList[i],geneList[k]=geneList[k],geneList[i]   if i<k
  end
  p left
  p right
  geneList[left],geneList[k]=geneList[k],geneList[left] if geneList[left].point<geneList[k].point
  GeneSort(geneList,left,k-1)
  GeneSort(geneList,k+1,right)
  return geneList
end
foo=Array.new(5){Gene.new(1)}
p foo
p GeneSort(foo,0,5-1)
