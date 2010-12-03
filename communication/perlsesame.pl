#!/usr/bin/perl

# read in our shit
open FILE, "temp.txt" or die $!;
open SENSORS, ">", "sensors.txt" or die $!;
open COMMANDS,">", "commands.txt" or die $!;

# process
while (my $line = <FILE>)
{
    # read line into array
    # delimiting by semicolons and commas
    @array = split(/ /, $line);

    pop (@array);
    foreach (@array)
    {
        if($_ =~ m/0AL/) {print COMMANDS "CMD_ADJUST_LEFT,"; print SENSORS "noHit,"; }
        if($_ =~ m/1AL/) {print COMMANDS "CMD_ADJUST_LEFT,"; print SENSORS "leftHit,"; }
        if($_ =~ m/2AL/) {print COMMANDS "CMD_ADJUST_LEFT,"; print SENSORS "rightHit,"; }
        if($_ =~ m/3AL/) {print COMMANDS "CMD_ADJUST_LEFT,"; print SENSORS "bothHit,"; }
        if($_ =~ m/512AL/) {print COMMANDS "CMD_ADJUST_LEFT,"; print SENSORS "goal,"; }

        if($_ =~ m/0AR/) {print COMMANDS "CMD_ADJUST_RIGHT,"; print SENSORS "noHit,"; }
        if($_ =~ m/1AR/) {print COMMANDS "CMD_ADJUST_RIGHT,"; print SENSORS "leftHit,"; }
        if($_ =~ m/2AR/) {print COMMANDS "CMD_ADJUST_RIGHT,"; print SENSORS "rightHit,"; }
        if($_ =~ m/3AR/) {print COMMANDS "CMD_ADJUST_RIGHT,"; print SENSORS "bothHit,"; }
        if($_ =~ m/512AR/) {print COMMANDS "CMD_ADJUST_RIGHT,"; print SENSORS "goal,"; }

        if($_ =~ m/0FW/) {print COMMANDS "CMD_FORWARD,"; print SENSORS "noHit,"; }
        if($_ =~ m/1FW/) {print COMMANDS "CMD_FORWARD,"; print SENSORS "leftHit,"; }
        if($_ =~ m/2FW/) {print COMMANDS "CMD_FORWARD,"; print SENSORS "rightHit,"; }
        if($_ =~ m/3FW/) {print COMMANDS "CMD_FORWARD,"; print SENSORS "bothHit,"; }
        if($_ =~ m/512FW/) {print COMMANDS "CMD_FORWARD,"; print SENSORS "goal,"; }

        if($_ =~ m/0RT/) {print COMMANDS "CMD_RIGHT,"; print SENSORS "noHit,"; }
        if($_ =~ m/1RT/) {print COMMANDS "CMD_RIGHT,"; print SENSORS "leftHit,"; }
        if($_ =~ m/2RT/) {print COMMANDS "CMD_RIGHT,"; print SENSORS "rightHit,"; }
        if($_ =~ m/3RT/) {print COMMANDS "CMD_RIGHT,"; print SENSORS "bothHit,"; }
        if($_ =~ m/512RT/) {print COMMANDS "CMD_RIGHT,"; print SENSORS "goal,"; }

        if($_ =~ m/0LT/) {print COMMANDS "CMD_LEFT,"; print SENSORS "noHit,"; }
        if($_ =~ m/1LT/) {print COMMANDS "CMD_LEFT,"; print SENSORS "leftHit,"; }
        if($_ =~ m/2LT/) {print COMMANDS "CMD_LEFT,"; print SENSORS "rightHit,"; }
        if($_ =~ m/3LT/) {print COMMANDS "CMD_LEFT,"; print SENSORS "bothHit,"; }
        if($_ =~ m/512LT/) {print COMMANDS "CMD_LEFT,"; print SENSORS "goal,"; }

        if($_ =~ m/0NO/) {print COMMANDS "CMD_NO_OP,"; print SENSORS "noHit,"; }
        if($_ =~ m/1NO/) {print COMMANDS "CMD_NO_OP,"; print SENSORS "leftHit,"; }
        if($_ =~ m/2NO/) {print COMMANDS "CMD_NO_OP,"; print SENSORS "rightHit,"; }
        if($_ =~ m/3NO/) {print COMMANDS "CMD_NO_OP,"; print SENSORS "bothHit,"; }
        if($_ =~ m/512NO/) {print COMMANDS "CMD_NO_OP,"; print SENSORS "goal,"; }

    }
    
}
close FILE;
close COMMANDS;
close SENSORS;
