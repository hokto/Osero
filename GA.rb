class Gene
  attr_accessor:number,:boardEva
  def initialize(number,n)
    @number=number
    @boardEva=Array.new(n)
    i=0
    File.open("./Gene/gene#{number}.txt") do|file|
      file.each_line(rt="") do|line|
        boardEva[i]=line.split(" ").map(&:to_i) 
      end
    end
    p @boardEva
  end
end

p Gene.new(0,10)
