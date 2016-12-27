e strict;
use File::Basename;
use FileHandle;
use Getopt::Long;

###########################################################################
#
#
# How to use:
#
# This program creates a lot of temporary files.  I suggest you create
# a new directory and run the program from there.
#
# The basic idea is that you write some assembly code, and designate
# the code you want repeated by putting #@ at the beginning of the
# lines.  The number of repetitions is specified using command line
# parameters --start, --stop, and --step.  These parameters are used
# in a for loop: for ($x = $start; $x <= $stop; $x += $step)
#
# NOTE: $x refers to the total number of repeated *instructions*, it
# does NOT refer to the number of times the entire block of code is
# repeated.  Thus, the following template
#
# rdtsc
# movl %eax, -8(%ebx)
# #@ addl $1, %ebx
# rdtsc
# 
# with the parameters --start 0 --stop 3 --step 1 
# will produce the following three assembly programs:
#
# rdtsc
# movl %eax, -8(%ebx)
# rdtsc
#
# rdtsc
# movl %eax, -8(%ebx)
# addl $1, %ebx
# rdtsc
#
# rdtsc
# movl %eax, -8(%ebx)
# addl $1, %ebx
# addl $1, %ebx
# rdtsc
#
#
# rdtsc
# movl %eax, -8(%ebx)
# addl $1, %ebx
# addl $1, %ebx
# addl $1, %ebx
# rdtsc
#
# However, the parameters --start 0 --stop 5 --step 1 with the
# following assembly:
#
# rdtsc
# movl %eax, -8(%ebx)
# #@ addl $1, %ebx
# #@ addl $1, %ecx
# rdtsc
#
# will produce the following code:
#
# rdtsc
# movl %eax, -8(%ebx)
# rdtsc
#
# rdtsc
# movl %eax, -8(%ebx)
# addl $1, %ebx
# addl $1, %ecx
# rdtsc
#
# rdtsc
# movl %eax, -8(%ebx)
# addl $1, %ebx
# addl $1, %ecx
# addl $1, %ebx
# rdtsc
#
# rdtsc
# movl %eax, -8(%ebx)
# addl $1, %ebx
# addl $1, %ecx
# addl $1, %ebx
# addl $1, %ecx
# rdtsc
#
# rdtsc
# movl %eax, -8(%ebx)
# addl $1, %ebx
# addl $1, %ecx
# addl $1, %ebx
# addl $1, %ecx
# addl $1, %ebx
# rdtsc
#
# This distinction is especially important when using instructions
# that must come in pairs (e.g., push and pop).  In such cases, be
# sure to set --step to a multiple of 2.
#
# If you assembly contains %i, %i will be replaced with the batch
# number for the code.  This will allow you to write templates with
# multiple labels.
#
# Finally, the code that parses the output of the comiled assembly
# file looks for the output of "Difference %u" on every line.
#
###########################################################################


my $source_file = shift || &usage();
my $stop = 10;
my $start =  0;
my $step =   1;
my $cleanUp = 1;

my $outputKeyword = "Difference";   # String to watch for when parsing output
my $warmUpIterations = 1000; # number of iterations to ignore.
my $iterationsToMeasure = 5000; # number of iterations to measure.

&GetOptions("start=i" => \$start,
	    "stop=i" => \$stop,
	    "step=i" => \$step,
	   "cleanUp!" => \$cleanUp,
    "outputKeyword" => \$outputKeyword,
    "warmUpIterations=i" => \$warmUpIterations,
    "iterationsToMeasure=i" => \$iterationsToMeasure);

my $prev_answer = " ";
for (my $x = $start; $x <= $stop; $x += $step) {
  &make_file($source_file, $x);
  &compile_file($source_file, $x);
  my $ans = &run_test($source_file, $x);
  
  if ($prev_answer eq " ") {
    print "$x\t$ans\n";
  } else {
    my $diff = $ans - $prev_answer;
    print "$x\t$ans\t$diff\n";
  }
  $prev_answer = $ans;
  &clean_up($source_file, $x) if ($cleanUp);
}

sub usage {
  print STDERR "Usage: time_tester.pl assembly_template.s [--start int] [--stop int] [--step int]\n";
  
  print STDERR "Your assembly template should have some lines that begin with #@.  These lines will be repeated as indicated by start, stop, and step.  Note:  The value of each loop refers to the total number of marked instructions, not the total number of times each block is repeated.\n";
  print STDERR "\nSee comment in the source code for more details.\n";
  exit(0);
}

#################################################################
#
# Convert file names of the form
#    name.suffix
# into 
#    name_iter.suffix
#
#################################################################

sub get_new_name  {
    my $source_file = shift;
    my $iter = shift;
    
    # The '\..*' syntax defines the suffix as the final dot and anything following.
    my ($name, undef, $suffix) = &File::Basename::fileparse($source_file, '\..*');
    return ("./${name}_${iter}${suffix}", "${name}_${iter}");
  }

sub clean_up
  {
    my $source_file = shift;
    my $iter = shift;

    my ($file_name, $prog_name) = &get_new_name($source_file, $iter);
    `rm ./$file_name`;
    `rm ./$prog_name`;
  }

#################################################################
#
# Run the sample program 
#
#################################################################
sub run_test {
    my $source_file = shift;
    my $iters = shift;
    my ($file_name, $prog_name) = &get_new_name($source_file, $iters);

    my %results;
    my $fh = FileHandle->new("./$prog_name|");

    # Warm up (throw out the first few results)
    for (my $x = 0; $x < $warmUpIterations; $x++)  {
      $fh->getline();
    }

    for (my $x = 0; $x < $iterationsToMeasure; $x++) {
      my $line = $fh->getline();
      if (!defined($line)) {
	print STDERR "$prog_name $x Unexpected end of Input.  (Did you trash your loop counter?)\n";
	last;
      } elsif ($line =~ /$outputKeyword\s+(\d+)/) {
	my $answer = $1;
	if (!defined($results{$answer})) {
	    $results{$answer} = 1;
	  } else {
	    $results{$answer}++;
	  }
      } else {
	print STDERR "Output line doesn't match expect format. ($outputKeyword)\n";
	    print STDERR $line;
	    exit 1;
	}
#	print STDERR "$iters -- $line"
	
      }
    $fh->close();
    #    my @ansvals = sort {$a <=> $b} keys (%results);
    # foreach my $ans (@ansvals) { print "$ans\t$results{$ans}\n";}
#    my @cycles_list = sort {$b <=> $a} values (%results);
 #   foreach my $key (keys %results)
 #     {
#	return $key if($results{$key} == $cycles_list[0]);
#      }

    # returns the fastest answer
    my @values = sort {$a <=> $b} keys (%results);

    # return -999 if none of the lines returned contain a valid measurement.
    return -999 if (@values < 1);
    return $values[0];
}

#################################################################
#
# Compile the given file using gcc
#
#################################################################	
sub compile_file {
    my $source_file = shift;
    my $iters = shift;
    my ($file_name, $prog_name) = &get_new_name($source_file, $iters);

    `gcc $file_name -o $prog_name`;

    # Quit if the compile was unsuccessful
    if ($? != 0) {
      print STDERR "Compilation error!\n";
      exit(0);
    }
  }


#################################################################
#
# Generate a test file by taking the source file and repeating the
# lines of code marked by #@ $iters times. (See note at the beginning
# of this file.
#
#################################################################

sub make_file {
  my $source_file = shift;
  my $iters = shift;

  # Inserts $iters into the filename before the suffix.
  my ($new_name) = &get_new_name($source_file, $iters);
  
  
  my $fhIn = FileHandle->new($source_file, "r") or die "Input file \"$source_file\" doesn't exist, or is not readable.\n";
  my $fhOut = FileHandle->new($new_name, "w") or die "Out: $!";

  my @comment_line;
  
  my $line;
  while($line = $fhIn->getline()) {
    my $i = 0;
    
    # remember all the lines that begin with #@
    # Note:  This remembers only a single group of 
    # marked lines.
    while ($line =~ /^\s*\#\@/) {
	$line =~ s/^\s*\#\@/  /;
	$comment_line[$i] = $line;		
	$line = $fhIn->getline();
	$i++;
      }

    # If we found some marked lines,
    # repeated those lines the specified number of times.
    if ($i > 0)  {
      for (my $j = 0; $j < $iters; $j++) {
	  my $line = $comment_line[$j % $i];

	  # replace a %i in the code with the particular
	  # iteration for this block
	  my $v = int ($j / $i);
	  $line =~ s/\%i/$v/;
	  
	  $fhOut->print($line);
	} # end for
    } # end if

    # prints the "non-special" line that ends the while loop above.
    $fhOut->print($line);
  } # end for each line
  
  $fhIn->close();
  $fhOut->close();
} # end make_file

