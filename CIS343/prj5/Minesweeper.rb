#
#  Minesweeper game
#
#  Author(s): Isaac Smith and Sam Essenburg
#

#
# module that defines global constants
#
module Constants
  WON = 0
  LOST = 1
  INPROGRESS = 2
  BOARD_SIZE_MIN = 5
  BOARD_SIZE_MAX = 15
  PCT_MINES_MIN = 10
  PCT_MINES_MAX = 70
end

#
# Cell class represents a cell on the minesweeper board
#
class Cell
  
  attr_accessor :is_mine, :nbr_mines, :visible
  
  def initialize(nbr_mines,is_mine,visible)
    self.nbr_mines=(nbr_mines)
    self.is_mine=(is_mine)
    self.visible=(visible)
  end
  
  def to_s
    "#@is_mine #@nbr_mines #@visible"
  end
end

#
# Minesweeper class represents the game board and contains game logic
#
class Minesweeper
  
  # initializes a Minesweeper object
  def initialize(board_size,percent_mines)
    @board_size = board_size
    @nbr_mines = (board_size * board_size * (percent_mines/100.0)).to_i
    
    # setup a 2-dimensional array of Cell objects
    @board = Array.new(board_size)
    @board.fill { |i| Array.new(board_size) }
    for i in 0...board_size
      for j in 0...board_size
        @board[i][j] = Cell.new(0,false,false)
      end
    end
    
    place_mines_on_board()
    fill_in_minecount_for_non_mine_cells()
  end
  
  # places mines randomly on the board
  def place_mines_on_board
    count = 0
    while count < @nbr_mines
      x = Random.rand(@board_size) 
      y = Random.rand(@board_size)
      
      if !@board[x][y].is_mine
        @board[x][y].is_mine = true
        count += 1
      end
    end
  end
  
  # for each non-mine cell on the board, set @nbr_mines of each Cell on the board
  # to the number of mines in the immediate neighborhood.
  def fill_in_minecount_for_non_mine_cells
    for i in 0...@board_size
      for j in 0...@board_size
        if !@board[i][j].is_mine
          @board[i][j].nbr_mines = get_nbr_neighbor_mines(i,j)
        end
      end
    end
  end
    
  # processes cell selection by user during the game
  # returns Constants::WON, Constants::LOST, or Constants::INPROGRESS
  def select_cell(row,col)
    @board[row][col].visible = true
    
    if @board[row][col].is_mine
      return Constants::LOST
    end
    if @board[row][col].nbr_mines == 0
      set_all_neighbor_cells_visible(row,col)
    end
    if @nbr_mines + nbr_visible_cells == @board_size*@board_size
      return Constants::WON
    end
    
    return Constants::INPROGRESS
  end
  
  # returns the number of mines in the immediate neighborhood of a cell
  # at location (row,col) on the board.
  def get_nbr_neighbor_mines(row,col)
    count = 0
    
    	#check left, diagupleft, above, diagupright, right
	if col > 0 && @board[row][col-1].is_mine
  		count += 1
  	end
  	if (col > 0 && row > 0) && @board[row-1][col-1].is_mine
  		count += 1
  	end
  	if (row > 0 && @board[row-1][col].is_mine)
  		count += 1
  	end
  	if (row > 0 && col < @board_size-1) && @board[row-1][col+1].is_mine
  		count += 1
  	end
  	if col < @board_size-1 && @board[row][col+1].is_mine 
  		count += 1
  	end
  	#check diagdownright, below, diagdownleft
  	if (row < @board_size-1 && col < @board_size-1) && @board[row+1][col+1].is_mine
  		count += 1
  	end
  	if row < @board_size-1 && @board[row+1][col].is_mine
  		count += 1
  	end
  	if (row < @board_size-1 && col > 0) && @board[row+1][col-1].is_mine
  		count += 1
  	end
  	return count
  end
  
  # returns the number of cells that are currently visible on the board
  def nbr_visible_cells
    count = 0
    
    for i in 0...@board_size
      for j in 0...@board_size
        if @board[i][j].visible
          count += 1
        end
      end
    end
    return count
  end
  
  # if the mine count of a cell at location (row,col) is zero, then make 
  # the cells in the immediate neighborhood visible and repeat this
  # process for each of the cells in this set of cells that have a mine
  # count of zero, and so on.
  def set_all_neighbor_cells_visible(row,col)
    #check left
    if col > 0 && !@board[row][col-1].visible
    	select_cell(row, col-1)
    end
    #check diag up left
    if (col > 0 && row > 0) && @board[row-1][col-1].visible
    	select_cell(row-1, col-1)
    end
    #check above
    if (row > 0 && !@board[row-1][col].visible)
    	select_cell(row-1, col)
    end
    #check diag up right
    if ((row > 0 && col < @board_size-1) && !@board[row-1][col+1].visible)
    	select_cell(row-1, col+1)
    end
    #check right
    if (col < @board_size-1 && !@board[row][col+1].visible)
    	select_cell(row, col+1)
    end
    #check diag down right
    if ((row < @board_size-1 && col < @board_size-1) && !@board[row+1][col+1].visible)
    	select_cell(row+1, col+1)
    end
    #check below
    if (row < @board_size-1 && !@board[row+1][col].visible)
    	select_cell(row+1, col)
    end
    #check diag down left
    if ((row < @board_size-1 && col > 0) && !@board[row+1][col-1].visible)
    	select_cell(row+1, col-1)
    end
  end
  
  # returns a string representation of the board
  def to_s(display_mines=false)
    str = ""
    for i in 0...@board_size
      str << (i == 0 ? sprintf("%6d",i+1) : sprintf("%3d",i+1))
    end
    str << "\n"
    
    for i in 0...@board_size
      str << sprintf("%3d",i+1)
      for j in 0...@board_size
        if @board[i][j].visible
          str << (@board[i][j].is_mine ? sprintf("  *") : sprintf("%3d", @board[i][j].nbr_mines))
        else
          str << (display_mines && @board[i][j].is_mine ? sprintf("  *") : sprintf("  ?"))
        end
      end
      str << "\n"
    end
    
    str
  end
  
  # make these methods private
  private :place_mines_on_board, :fill_in_minecount_for_non_mine_cells,
        :get_nbr_neighbor_mines, :nbr_visible_cells, 
        :set_all_neighbor_cells_visible

end

#
# main method that provides the user interface to the game
#
def main
  display_mines = false
  game_state = Constants::INPROGRESS
  
  board_size = get_board_size()
  percent_mines = get_percent_mines()
  board = Minesweeper.new(board_size,percent_mines)
  puts board.to_s(display_mines)
  
  while true do
    print "Enter command (m/M for menu): "
    command = STDIN.gets.strip
    case command
      when 'm' || 'M'
        display_menu()
        
      when 'c' || 'C'
        row = 0
        col = 0
        while row < 1 || row > board_size || col < 1 || col > board_size do
          print "Enter row and column of cell: "
          tokens = STDIN.gets.strip.split(' ')
          row = tokens[0].to_i
          col = tokens[1].to_i
          if row < 1 || row > board_size || col < 1 || col > board_size
            puts "Invalid row or column values. Try again."
          end
        end
        game_state = board.select_cell(row-1,col-1)
        puts board.to_s(display_mines)
        
      when 's' || 'S'
        display_mines = true
        puts board.to_s(display_mines)
        
      when 'h' || 'H'
        display_mines = false
        puts board.to_s(display_mines)
        
      when 'b' || 'B'
        puts board.to_s(display_mines)
        
      when 'q' || 'Q'
        puts "Bye."
        return
        
      else
        puts "Invalid command. Try again."
    end # end case
    
    if game_state == Constants::WON
      puts "You found all the mines. Congratulations. Bye."
      return
    elsif game_state == Constants::LOST
      puts "Whoops. you landed on a mine. Bye"
      return
    end
  end   # end while
  
end

#
# Displays command menu
#
def display_menu
  puts "List of available commands:"
  puts "   Show Mines: s/S"
  puts "   Hide Mines: h/H"
  puts "   Select Cell: c/C"
  puts "   Display Board: b/B"
  puts "   Display Menu: m/M"
  puts "   Quit: q/Q"
end

#
# Prompts the user for board @board_size, reads and validates the input
# entered, and returns the integer if it is within valid range.
# repeats this in a loop until the user enters a valid value.
#
def get_board_size
  @board_size = 0
  while @board_size < Constants::BOARD_SIZE_MIN || @board_size > Constants::BOARD_SIZE_MAX do
    print "Enter board @board_size (#{Constants::BOARD_SIZE_MIN} .. #{Constants::BOARD_SIZE_MAX}): "
    @board_size = STDIN.gets.strip.to_i
    if (@board_size < Constants::BOARD_SIZE_MIN || @board_size > Constants::BOARD_SIZE_MAX) 
      puts "Invalid board @board_size. Try again."
    end
  end
  @board_size
end

#
# Prompts the user for percentage of mines to place on the board,
# reads and validates the input entered, and returns the integer if it
# is within valid range. repeats this in a loop until the user enters
# a valid value for board @board_size.
#
def get_percent_mines
  percent_mines = 0
  while percent_mines < Constants::PCT_MINES_MIN || percent_mines > Constants::PCT_MINES_MAX do
    print "Enter percentage of mines on the board (#{Constants::PCT_MINES_MIN} .. #{Constants::PCT_MINES_MAX}): "
    percent_mines = STDIN.gets.strip.to_i
    if (percent_mines < Constants::PCT_MINES_MIN || percent_mines > Constants::PCT_MINES_MAX)
      puts "Invalid value for percentage. Try again."
    end
  end
  percent_mines
end
 
#
# invoke main
#
main()
