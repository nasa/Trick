import trick

def main():
    exec(open("Modified_data/fooChange2.dr").read())

    trick.add_read(5.0, 'trick.load_checkpoint("RUN_test8/chkpnt_5.000000")') # this checkpoint does not contain data recording

    trick.stop(20.0)

if __name__ == "__main__":
    main()