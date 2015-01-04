import calendar
import datetime
import locale
import os
import sys

def read(filename):
    with open(filename) as f:
        return f.read()

def write(string, filename):
    if os.path.isfile(filename):
        if os.path.getsize(filename) == len(string):
            if read(filename) == string:
                return

    with open(filename, "w") as f:
        f.write(string.encode("utf-8"))

locale.setlocale(locale.LC_TIME, "nb_NO.UTF-8")

filename = sys.argv[1]
basename = filename.split(".")[0]
year, month = map(int, basename.split("-"))

sections = []
for line in open(filename).readlines():
    if "\n" == line[0]:
        continue

    words = line.split()

    if 1 == len(words):
        sections.append([words[0], []])
        continue

    day, start, end = map(int, words)
    date = datetime.date(year, month, day)
    _, week, weekday = date.isocalendar()
    wdn = calendar.day_abbr[weekday - 1]
    wdn = unicode(wdn.decode("utf-8"))
    session = (week, wdn, day, start, end, end - start)

    weeks = sections[-1][1]

    if weeks:
        if weeks[-1][0][0] != session[0]:
            weeks.append([])
    else:
        sections[-1][1].append([])

    sections[-1][1][-1].append(session)

tex = ""
tex += """\\documentclass[a4paper,norsk,12pt]{article}

\\usepackage[norsk]{babel}
\\usepackage[usenames,dvipsnames]{color}
\\usepackage[utf8]{inputenc}
\\usepackage{booktabs}
\\usepackage{geometry}
\\usepackage{longtable}

\\geometry{left=2.0cm,right=2.0cm,top=2.0cm}

\\begin{document}

\\begin{center}
\\huge\n"""

tex += "Timeliste \\hfill %s %d\n" % (calendar.month_name[month].capitalize(), year)
tex += "\\end{center}\n"

for s in sections:
    name, weeks = s
    total = 0

    tex += "\n\section*{%s}\n\n" % name
    tex += "\\begin{longtable}{ccrcrrl}\n"
    tex += "\\textbf{Dato} & & \\textbf{Start} &  & \\textbf{Slutt} & \\textbf{Timer}\\\\\n"
    tex += "\\toprule\n"

    for i in range(len(weeks)):
        week = weeks[i]

        for d in week:
            total += d[5]
            tex += "%s \\hfill %2d. & & %02d:00 & --- & %02d:00 & %d \\\\\n" % d[1:]

        if i + 1 < len(weeks):
            tex += "\\\\\n"

    tex += "\\toprule\n"
    tex += "\\textbf{Totalt} & & & & & %d &\n" % total
    tex += "\\end{longtable}\n"

tex += "\\end{document}\n"

write(tex, basename + ".tex")
