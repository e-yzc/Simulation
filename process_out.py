import os
dir = os.path.dirname(__file__)
sim_file = os.path.join(dir, 'Release','sim_results.csv')


import pandas as pd 
import numpy as np

data = pd.read_csv(sim_file)
data.columns = data.columns.str.strip().str.lower().str.replace(' ', '_').str.replace('(', '').str.replace(')', '')

train_corr = np.corrcoef(data["zt"], data["ft"])
test_corr = np.corrcoef(data["zpt"], data["ft2"])

train_corrcoef = train_corr[0,1]
test_corrcoef = test_corr[0,1]


print(f"Training corrcoef: {train_corrcoef}, testing corr: {test_corrcoef}")

outfile_path = os.path.join(dir, "script_out.csv")
outfile = open(outfile_path, 'a')
outfile.write(f"{train_corrcoef},{test_corrcoef}\n")
outfile.close()
