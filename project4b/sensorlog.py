#!/usr/bin/python
#
# process a sensor log and decide whether or not the reports
# are consistent with commind line parameters
#
# usage: xxx [--scale=C/F] [--period=#] [--start] [--min=#] [--max=#] logfile
#
# understood commands in log
#   START
#   STOP
#   SCALE=C/F
#   PERIOD=seconds
#   SHUTDOWN
#
# stdout:   any specific complaints
#
# exit code
#   0   all input was consistent with parameters
#   1   inconsistencies were detected
#   -1  something went wrong
#
import sys
import os.path
import argparse

exitCode = 0


def checkPeriod(reports, interval, period):

    expected = interval / period
    if (reports < expected - 1) or (reports > expected + 1):
        sys.stdout.write("ERROR: %d reports in %d seconds (expected %d)\n" %
                         (reports, interval, expected))
        global exitCode
        exitCode = 1


#
# process the input file,
#
def process(opts):
    # current configuration state
    scale = opts.scale
    period = opts.period
    reporting = True    # are we currently reporting
    minValue = opts.minValue
    maxValue = opts.maxValue
    deltaT = 5

    # statistics
    line = 1
    start = 0           # starting time for this interval
    last = 0            # time of the last report
    reports = 0         # reports in this interval
    totReports = 0      # total reports in log
    calibrated = False  # do we have a first reading
    sawOff = False      # have we seen an OFF
    sawShutdown = False     # have we seen a SHUTDOWN
    sawStart = False    # have seen a START
    sawStop = False     # have seen a STOP
    sawScale = False    # have seen a SCALE
    sawPeriod = False   # have seen a PERIOD
    measuredPeriods = False # got to measure rates

    global exitCode

    # make sure the input file is valid
    if not os.path.isfile(opts.logfile):
        sys.stderr.write("Unable to open log %s\n" % (opts.logfile))
        exitCode = 1
        return

    # process each line in the file
    infile = open(opts.logfile)
    for l in infile:
        # split the line into fields, and possible time stamp
        fields = l.split(' ')

        # figure out what fields we have
        if len(fields) > 1:
            time = fields[0].split(':')
            if len(time) == 3:  # multiple fields w/time
                try:
                    h = int(time[0])
                    m = int(time[1])
                    s = int(time[2])
                    last = s + ((m + (h * 60)) * 60)

                    # skip ahead to the first non-blank field
                    f = 1
                    while f < len(fields) and not fields[f]:
                        f += 1
                    if f >= len(fields) or not fields[f]:
                        report = ""
                    else:
                        report = fields[f]

                except ValueError:  # not an integer
                    report = ""
                    sys.stderr.write("%d: %s ... INVALID TIME VALUE\n" % (line, fields[0]))

            else:               # multiple fields w/o time
                h = 0
                m = 0
                s = 0
                report = fields[0]
        else:                   # one field only
            h = 0
            m = 0
            s = 0
            report = fields[0]

        secs = 0 if start == 0 else last - start

        if report.startswith("ID="):
            if opts.verbose:
                sys.stderr.write("%s: %d\n" % (line, report))
        elif report.startswith("START"):
            if opts.verbose:
                sys.stderr.write("%d: START\t(%d reports in %d seconds)\n" %
                                 (line, reports, secs))
            if not reporting:
                start = 0
            reports = 0
            reporting = True
            sawStart = True

        elif report.startswith("STOP"):
            if opts.checkStart and reporting is True and reports == 0:
                sys.stdout.write("ERROR: 0 reports after START\n")
                exitCode = 1
            if opts.verbose:
                sys.stderr.write("%d: STOP\t(%d reports/%ds)\n" %
                                 (line, reports, secs))

            # check the reporting periodicity
            if opts.checkPeriod and reporting:
                checkPeriod(reports, secs, period)
                if reports > 1:
                    measuredPeriods = True

            # and reset the reporting status
            start = last
            reports = 0
            reporting = False
            sawStop = True

        elif report.startswith("PERIOD="):
            # check the reporting periodicity
            if opts.checkPeriod and reporting:
                checkPeriod(reports, secs, period)
                if reports > 1:
                    measuredPeriods = True

            # set the new reporting period
            args = report.split('=')
            period = int(args[1])
            if opts.verbose:
                sys.stderr.write("%d: PERIOD=%d\t(%d reports/%d seconds)\n" %
                                 (line, period, reports, secs))
            start = 0
            reports = 0
            sawPeriod = True

        elif report.startswith("SCALE="):
            args = report.split('=')
            newscale = args[1][0]
            if scale == 'F' and newscale == 'C' and minValue is not None:
                scale = newscale
                minValue = (minValue - 32) * 5 / 9
                maxValue = (maxValue - 32) * 5 / 9
            elif scale == 'C' and newscale == 'F' and minValue is not None:
                scale = newscale
                minValue = (minValue * 9/5) + 32
                maxValue = (maxValue * 9/5) + 32
            if opts.verbose:
                sys.stderr.write("%d: SCALE=%s\t(limits=%d-%d)\n" %
                                 (line, scale, minValue, maxValue))
            sawScale = True

        elif report.startswith("OFF"):
            if opts.verbose:
                sys.stderr.write("%d: OFF\t(%d reports/%d seconds)\n" %
                                 (line, reports, secs))

            # check the reporting periodicity
            if opts.checkPeriod and reporting:
                checkPeriod(reports, secs, period)
                if reports > 1:
                    measuredPeriods = True

            reporting = False
            sawOff = True
            reports = 0

        elif report.startswith("SHUTDOWN"):
            if opts.verbose:
                sys.stderr.write("%d: SHUTDOWN\t(%d reports/%d seconds)\n" %
                                 (line, reports, secs))

            # check the reporting periodicity
            if opts.checkPeriod and reporting:
                checkPeriod(reports, secs, period)
                if reports > 1:
                    measuredPeriods = True

            sawShutdown = True
            reports = 0

        elif report.startswith("LOG"):
            if opts.verbose:
                sys.stderr.write("%d: %s" % (line, l))

        else:   # it is most likely a data report
            try:
                v = float(report)

                # see if we have our first valid timestamped report
                if start == 0:
                    start = last
                else:
                    reports += 1
                    totReports += 1

                # use first plausible value to set range
                if minValue is not None:
                    if opts.checkTemps and (v < minValue or v > maxValue):
                        sys.stdout.write("ERROR: %d out of range (%d-%d%s)\n" %
                                         (v, minValue, maxValue, scale))
                        exitCode = 1
                    elif not calibrated:
                        minValue = v - deltaT   # tighten range
                        maxValue = v + deltaT   # tighten range
                        calibrated = True

                if opts.verbose:
                    sys.stderr.write("%d: SAMPLE %02d:%02d:%02d %f\n" %
                                     (line, h, m, s, v))

                if (opts.checkStop or opts.checkOff) and reporting is False and reports > 1:
                    sys.stdout.write("ERROR: %d reports after STOP/OFF\n" %
                                     (reports))
                    exitCode = 1

            except ValueError:  # not a temperature
                if len(l) > 1:
                    sys.stderr.write("UNRECOGNIZED INPUT: %s\n" % (l))
        line += 1

    # see if we were checking the period, and saw no period operations
    if opts.checkPeriod and not sawPeriod:
        sys.stderr.write("no PERIOD operations in log\n")
        exitCode = 1

    # see if we were checking period, but didn't have enough samples
    if opts.checkPeriod and not measuredPeriods:
        sys.stderr.write("insufficient operations to measure PERIODs\n")
        exitCode = 1

    # see if we were checking starts, but saw none
    if opts.checkStart and not sawStart:
        sys.stderr.write("no START operations in log\n")
        exitCode = 1

    # see if we were checking stops, but saw none
    if opts.checkStop and not sawStop:
        sys.stderr.write("no STOP operations in log\n")
        exitCode = 1

    # see if we were checking off, but saw none
    if opts.checkOff and not sawOff:
        sys.stderr.write("no OFF operations in log\n")
        exitCode = 1

    # see if we were checking temperatures, but didn't have enough samples
    if opts.checkTemps and totReports < 2:
        sys.stderr.write("insufficient reports to verify\n")
        exitCode = 1

    # see if the off was followed by a shutdown
    if opts.checkOff and sawOff and not sawShutdown:
        sys.stderr.write("OFF but no SHUTDOWN\n")
        exitCode = 1

#
# parse command line options and process the input file
#
if __name__ == "__main__":
    # process command line parameters
    parser = argparse.ArgumentParser()
    parser.add_argument('--period', dest='period', type=int, action='store',
                        default=1)
    parser.add_argument('--scale', dest='scale', action='store', default='F')
    parser.add_argument('--min', dest='minValue', type=float, action='store',
                        default=None)
    parser.add_argument('--max', dest='maxValue', type=float, action='store',
                        default=None)
    parser.add_argument('--verbose', dest='verbose', action='store_true',
                        default=False)
    parser.add_argument('--check', dest='checks', action='store',
                        default="temp,period,scale,stop,start,off")
    parser.add_argument('logfile')
    opts = parser.parse_args()

    # default temperature range
    if opts.minValue is None:
        opts.minValue = 32.0 if opts.scale is 'F' else 0.0
    if opts.maxValue is None:
        opts.maxValue = 120.0 if opts.scale is 'F' else 40.0

    # figure out which checks to enable
    opts.checkTemps = 'temp' in opts.checks
    opts.checkPeriod = 'period' in opts.checks
    opts.checkStart = 'start' in opts.checks
    opts.checkStop = 'stop' in opts.checks
    opts.checkOff = 'off' in opts.checks

    if opts.verbose:
        sys.stderr.write("Options: period=%d, scale=%s, min=%d, max=%d\n" %
                         (opts.period, opts.scale, opts.minValue,
                          opts.maxValue))
        sys.stderr.write("Checks: ")
        comma = ''
        if opts.checkTemps:
            sys.stderr.write("%stemps" % (comma))
            comma = ','
        if opts.checkPeriod:
            sys.stderr.write("%speriod" % (comma))
            comma = ','
        if opts.checkStart:
            sys.stderr.write("%sstart" % (comma))
            comma = ','
        if opts.checkStop:
            sys.stderr.write("%sstop" % (comma))
            comma = ','
        if opts.checkOff:
            sys.stderr.write("%soff" % (comma))
            comma = ','
        sys.stderr.write("\n")

    # open and process the file
    if (opts.logfile is not None):
        process(opts)

    if opts.verbose:
        sys.stderr.write("Exit status: %d\n" % (exitCode))
    exit(exitCode)
