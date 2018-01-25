##Author: Ryan Bomalaski
##Author ID:
##File: HW1.py
##Class: SWE 5001
##Section: 1
##Institution: Florida Institute of Technology
##
##Purpose:  Take a set of numbers and return a statistical report containing the
##following - Mean, Median, Mode/Modes, Dataset Minimum, Dataset Maximum, 
##Dataset Range, Dataset Standard Deviation, an Dataset Frequency Table
##
##Stepwise Function:  
##    1. Prompt user for number of reports, datasets to use and names of tests.
##    2. Return a report for each of the in spec statistical analysis requested
##    3. Prompt the user to see if they would like to save to text file
##    4. Repeat if they want to, Quit if they don't
##
##Other files referenced/used in this one:
##    1.  Used http://patorjk.com/software/taag/ to make a fun 
##        title and spice this up :)
##    2.  importing math - to do the squareroot required for stan dev
##    3.  importing datetime - for name files uniquely without worrying
##        about file location or overwriting existing files.
##    
##
##File name of Design Document:
##    1. Test Report - HW1.docx
##          renamed BOMALATR for turning in assignment
##    2. Primary Design - HW1.txt
##          contains any typed design work
##
##Future Features to consider:
##    1. Input of a text file for processing a large dataset
##    2. Improve floating point handling
##
##This program assumes no copyright of any referenced work, and any usage of 
##copyrighted work is being done under Fair Use for educational purposes or with 
##respect to the license of the work.  Where outside work may have been used
##either explicitly or for inspiration, it has been listed in the "Other files
##referenced/used in this one:" section of the header.

import math
from datetime import datetime

def stat_reporter():
    ## ""Main"" function for this program.  Calling stat_reporter() will begin
    ##the whole dern thing.  A call has been added to the end of the program,
    ##to allow for quick execution during grading.  This will be removed as I
    ##adapt this tool past just classroom use.
    
    ##Initialized final out put at the beginning to keep the ASCII art together
    ##This keeps only one area of the code looking strange due to the letters
    ##ASCII Art generated using http://patorjk.com/software/taag/
    final_formatted_report = '''__   __                ______                      _     
\ \ / /                | ___ \                    | |  _ 
 \ V /___  _   _ _ __  | |_/ /___ _ __   ___  _ __| |_(_)
  \ // _ \| | | | '__| |    // _ | '_ \ / _ \| '__| __|  
  | | (_) | |_| | |    | |\ |  __| |_) | (_) | |  | |_ _ 
  \_/\___/ \__,_|_|    \_| \_\___| .__/ \___/|_|   \__(_)
                                 | |                     
                                 |_|                     '''
    print('''

   _____ _        _     _____                       _            
  / ____| |      | |   |  __ \                     | |           
 | (___ | |_ __ _| |_  | |__) |___ _ __   ___  _ __| |_ ___ _ __ 
  \___ \| __/ _` | __| |  _  // _ \ '_ \ / _ \| '__| __/ _ \ '__|
  ____) | || (_| | |_  | | \ \  __/ |_) | (_) | |  | ||  __/ |   
 |_____/ \__\__,_|\__| |_|  \_\___| .__/ \___/|_|   \__\___|_|   
                                  | |                            
                                  |_|                            
                  v.1.0 by Ryan Bomalaski
                  
Stat Reporter is a quick python application for generating very simple
statistical reports. Here you can choose to use one of the preset demo
data sets or enter your own.

NOTE!  To deal with floating point numbers, this program is only accurate
to the first 2 decimal places.  Work will be done in the future to detect
the correct number of decimal digits to round to.'''+'\n')
    number_of_report = 0
    while number_of_report == 0:
        try:
            number_of_report = int(input("How many reports will you be generating? "))
        except ValueError:
            print("Something went wrong.  Let's try again.")
            print("Be sure to enter only the number of reports.\n")
            
    for i in range(number_of_report):
        user_input = 0
        while user_input == 0:
            print("\nThis is report number " + str(i+1))
            report_name = str(input("What is the name of this report? "))
            set_correct = 'n'
            while (set_correct.lower() != 'y'):
                data_set = process_user_input()
                print('\n'+str(data_set)+' is your dataset.')
                set_correct = str(input("Does this look correct? [y/n]"))
                if (set_correct.lower() != 'y'):
                    print("\nOk, let's try it again")
        report1 = report(report_name)
        final_formatted_report += "\n" + report1.generate_report(data_set)
    print(final_formatted_report)
    ##currently prints the formatted report to the command line as a sort of
    ##final output

    print_decider = 0;
    while print_decider == 0:
        user_input = str(input("Would you like to output this report as a text file? [y/n]"))
        if user_input.lower() == 'y':
            ##Write final_formatted_report to txt file
            report_file_name = str(datetime.now())+'.txt' ##Ensures that a unique file is written for each report
            report_file = open(report_file_name, 'w+')
            report_file.write(final_formatted_report)
            print('''A file named ''' + report_file_name + ''' was created in this
program's directory.''')
            ##Name of file should be based on date and the number of tests
            break
        elif user_input.lower() == 'n':
            break
        else:
            print("I didn't understand that command.\n")
    
#This grabs the user's input list and makes it into a list of Float
##Later on I'd like to add the funcitonality to take in a text document
def process_user_input():
    user_input_dataset = []
    print("\n"+'''Please input your entire list. You will be asked for
your delimiter afterwards''')
    input_string = ''
    while input_string == '':
        input_string = str(input("Please input your list: "))
        input_string_delimiter = str(input("Please input your delimiter:"))
    ##Split the input string by the delimiter
    if input_string_delimiter != '':
        list_string_numbers = input_string.split(input_string_delimiter)
    else:
        list_string_numbers = input_string.split()
    list_float_numbers = [] ##what will be returned
    try:
        for i in range(len(list_string_numbers)):
            ##Attempts to convert it to float
            ##If it can't, it requests you to reenter your list
            ##DOES NOT PINPOINT ISSUE.  Might add functionality later
            number_entry = float(list_string_numbers[i])
            list_float_numbers.append(number_entry)
    except ValueError:
        ##If the data is entered with a space, or delimiter at the end
        ##Then a blank string is tacked on.  In the event of an error
        ##This attempts to remove that problem, and then try again.
        try:
            list_string_numbers.remove('')
            list_float_numbers = []
            for i in range(len(list_string_numbers)):
                ##Attempts to convert it to float
                ##If it can't, it requests you to reenter your list
                ##DOES NOT PINPOINT ISSUE.  Might add functionality later
                number_entry = float(list_string_numbers[i])
                list_float_numbers.append(number_entry)
        except ValueError:   
            print("\nSomething in your list didn't work right, please try again")
            list_float_numbers = process_user_input()
    return list_float_numbers
    
class report:
    'Each report will be treated as a class now'
    ## Initializes the class
    def __init__(self, report_name):
        self.report_name = report_name
        
    ##calculate the mean
    def dataset_mean(self, list_of_numbers):
        mean = sum(list_of_numbers)/len(list_of_numbers)
        return round(mean,2)

    ##calculate the median
    def dataset_median(self, list_of_numbers):
        sorted_list = sorted(list_of_numbers)
        if (len(sorted_list)%2 == 0):
            median = (sorted_list[int(len(sorted_list)/2)]
                      + sorted_list[int((len(sorted_list)/2)-1)])/2
        else:
            median = sorted_list[int(len(sorted_list)/2)]
        return round(median,2)

    ##calculate the mode
    def dataset_mode(self, list_of_numbers):
        countdict = {}
        for item in list_of_numbers:
            if item in countdict:
                countdict[item] += 1
            else:
                countdict[item] = 1
        countlist = countdict.values()
        maxcount = max(countlist)
        modelist = []
        for i in countdict:
            if countdict[i] == maxcount:
                modelist.append(i)
        return modelist
        
    ##calculate the min value
    def dataset_min(self, list_of_numbers):
        smallest = min(list_of_numbers)
        return smallest
            
    ##calculate the max value
    def dataset_max(self, list_of_numbers):
        largest = max(list_of_numbers)
        return largest
        
    ##calculate the range
    def dataset_range(self, list_of_numbers):
        rangeNum = max(list_of_numbers) - min(list_of_numbers)
        return round(rangeNum,2)
        
    ##calculate standard deviation
    def dataset_stan_dev(self, list_of_numbers):
        if (len(list_of_numbers) < 2):
            return "Error"
        the_mean = self.dataset_mean(list_of_numbers)
        total = 0
        for i in list_of_numbers:
            difference = i - the_mean
            diffsq = difference**2
            total += diffsq
        stand_dev = math.sqrt(total/(len(list_of_numbers)-1))
        return round(stand_dev,2)
        
    ##datasetFreqTable
    def dataset_freq_table(self, list_of_numbers):
        count_dict = {}
        sorted_list_numbers = sorted(list_of_numbers)
        sorted_list_unique = sorted(list(set(sorted_list_numbers)))
        frequency_table_string = "Frequency Table: \n"
        for i in sorted_list_numbers:
            if i in count_dict:
                count_dict[i] += 1
            else:
                count_dict[i] = 1
        for i in sorted_list_unique:
            this_line = str(i)
            dot_string = '.'
            for j in range(20-len(str(i))-len(str(count_dict[i]))):
                dot_string += '.'
            this_line += dot_string + str(count_dict[i])
            this_line = "        " + this_line
            frequency_table_string += this_line
            frequency_table_string += '\n'
        return frequency_table_string

    #Report Generator
    def generate_report(self, list_of_numbers):
        if len(list_of_numbers) == 0:
            error_message = ("Test " + self.report_name +" failed."
                             +"\nNull dataset provided.\n")
            return error_message
        else:
            ##Create a list of values, so my report
            ##can iterate through a loop to make parts
            ##much pretier
            list_of_values = ["Test Name:$$"
                              + self.report_name]
            ##Mean is index 1
            list_of_values.append("Mean is$$"
                                  + str(self.dataset_mean(list_of_numbers)))
            ##Median is index 2
            list_of_values.append("Median is$$"
                                  + str(self.dataset_median(list_of_numbers)))
            ##Mode is index 3
            list_of_values.append("Mode is$$"
                                  + str(self.dataset_mode(list_of_numbers)))
            ##Min is index 4
            list_of_values.append("Minimum Value is$$"
                                  + str(self.dataset_min(list_of_numbers)))
            ##Max is index 5
            list_of_values.append("Maximum Value is$$"
                                  + str(self.dataset_max(list_of_numbers)))
            ##Range is index 6
            list_of_values.append("Range is$$"
                                  + str(self.dataset_range(list_of_numbers)))
            ##Standard Deviation is index 7
            list_of_values.append("Standard Deviation is$$"
                                  + str(self.dataset_stan_dev(list_of_numbers)))
            ##Loop to add pretty portions for each value to the report:
            formatted_report = '\n' ##Blank report variable to append to
            for i in range(len(list_of_values)):
                raw_name = list_of_values[i]
                raw_pieces = raw_name.split("$$")
                dot_string = ''
                for j in range(84-len(raw_name)-2):
                    dot_string+= '.'
                pretty_name = raw_pieces[0] + dot_string + raw_pieces[1]+"\n"
                formatted_report += pretty_name
            ##tack on frequency table
            formatted_report += self.dataset_freq_table(list_of_numbers)
            ##Return formatted text  
            return formatted_report

stat_reporter()
##print("Stat Reporter v1.0 has been loaded into memory.")
##print("To get started, type stat_reporter() and press enter")
