from rabaDB.rabaSetup import *
import argparse
import os

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Statistics process.')
    parser.add_argument('files', metavar='f', type=argparse.FileType('r'),
                        nargs='+')
    parser.add_argument('--window', metavar='w', nargs='?', const=30, type=int,
                        default=0)
    parser.add_argument('--remove_old', nargs='?', const=False, default=True)
    args = parser.parse_args()
    if (args.remove_old):
        try:
            os.remove(".statistics.db")
        except OSError:
            print('No database found')
    RabaConfiguration('statistics', '.statistics.db')
    from statisticsHelper import *
    if (args.window != 0):
        # Look for all the created db and generate the json files, with the
        # given window.
        generateJSON(args.files, args.window)
    else:
        # Generate all the db from the file list
        parseFiles(args.files)
        generateJSON(args.files, args.window)
