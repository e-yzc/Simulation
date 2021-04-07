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


train_rmse = np.sqrt(sum([ (zt -ft)**2 for zt,ft  in zip(data["zt"], data["ft"]) ]) /len(data["zt"]))
test_rmse = np.sqrt(sum([ (zpt -ft2)**2 for zpt,ft2  in zip(data["zpt"], data["ft2"]) ]) /len(data["zpt"]))


print(f"Training corrcoef: {train_corrcoef}, testing corr: {test_corrcoef}")
print(f"Train RMSE: {train_rmse}, test RMSE: {test_rmse}")

outfile_path = os.path.join(dir, "script_out.csv")
outfile = open(outfile_path, 'a')
outfile.write(f"{train_corrcoef},{test_corrcoef}, {train_rmse}, {test_rmse}\n")
outfile.close()
