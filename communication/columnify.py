#!/usr/bin/python

#
# Read everything into results.txt into a 2D list
#
# results.txt is expected to be one column of values
# separated by lines that contain the word 'end'
#
# Example of expected input:
#             1
#             2
#             3
#             4
#             end
#             55
#             66
#             777
#             end
#             888
#             end
#             9
#             8
#             55
#             333
#             1111
#             end
#
#
# @author Andrew Nuxoll
# @version 06 April 2011


with open('results.txt', 'r') as f:
    i = 0                       # current row
    output = []                 # the 2D list we're filling
    numRows = 0;                # number of rows in the list (0 indexed)
    numCols = 0;                # number of columns in the list (0 indexed)
    for line in f:
        #If the line contains "end" then start a new column
        if line.find("end") != -1:
            i = 0
            numCols = numCols + 1
        #Otherwise add it to the current column
        else:
            
            # If this is a brand new row, initialize it with an empty list
            if numRows <= i:
                output.append([])
                numRows = numRows + 1
                
            # Insert blank entries into the list as necessary so that
            # the next number ends up in the proper column
            while len(output[i]) < numCols:
                output[i].append('')

            # remove the newline char and append to the list
            line = line.replace('\n', '')
            output[i].append(line)

            # update the index of the current row
            i = i + 1
f.closed


#
# Write out the 2D list as a CSV file named results.csv
#
with open('results.csv', 'w') as f:
    for list in output:
        for entry in list:
            f.write(entry + ','),
        f.write("\n")
f.closed
