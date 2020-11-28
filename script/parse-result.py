import os
import sys
import math
import random
import numpy as np

def parse(infiles, outfile):
	total = {}
	M = 1000000
	K = 1000
	for infile in sorted(infiles):
		with open(indir + infile, 'r') as fin:
			prev = {}
			results = {}
			name = infile.split('.')[0]
			if '_' in name: name = name.split('_')[0]
			for line in fin:
				size, ts = line.split(']')
				size = size.replace('[','')
				ts = int(ts.split(':')[1].strip())
				type = "W"
				if '_' in size: 
					size, type = size.split('_')
				size = int(size)

				if size < 100:
					if size < 20: 
						size -= 10
						type = "W"
					else:
						size -= 20
						type = "R"
					size = math.pow(10, 2+size)
					
				if size >= M: size = str(int(size/M)) + "M"
				elif size >= K: size = str(int(size/K)) + "K"
				size += "_" + type
				if size not in prev:
					prev[size] = ts
				else:
					if size not in results: results[size] = []
					results[size].append(ts - prev[size])
					del prev[size]
			
			for size, values in sorted(results.items(), key=lambda x: x[0]):
				if size not in total: total[size] = []

				_min, _max = np.min(values), np.max(values)
				mean = (np.sum(values) - _max - _min) / float(len(values)-2)
				total[size].append( mean )
			if "name" not in total: total["name"] = []
			total["name"].append(name)
	def toStr(values):
		strs = ""
		for avg in values:
			strs += "," + str(avg)
		return strs
	
	types = ["W", "R"]
	for type in types:
		with open(indir + outfile + "_" + type + ".csv", 'w') as fout:
			lst = []
			
				# fout.write(','.join([str(size)] + values) + '\n')
			lst = zip(*lst)
			# print(lst)
			values = total["name"]
			values = list(values)
			values.append("graphene3")
			values = sorted(values)
			fout.write("#name" + toStr(values) + '\n')

			
			keys = ["1K","10K","100K","1M","10M",]
			for key in keys:
				values = total[key + "_" + type]
				values = [values[0]] + [0] + values[1:]
				fout.write(str(key) + toStr(values) + '\n')

def parseNew(infiles, outfile):
	total = {}
	raw = {}
	
	KiB = 1024
	MiB = 1024 * 1024
	KB = 1000
	MB = 1000 * 1000
	totalLen = None
	sizes = set()
	for infile in sorted(infiles):
		with open(indir + infile, 'r') as fin:
			prev = {}
			results = {}
			name = infile.split('.')[0]
			if '_' in name: name = name.split('_')[0]
			lines = fin.readlines()
			for i, line in enumerate(lines):
				if line.startswith("[write_"): type = "W"
				elif line.startswith("[read_"): type = "R"
				else: continue
				size = int(line.split(']')[0].split('_')[-1])
				elapsedTime = float(line.split(':')[1].strip())
				
				if size % 10 == 0:
					K, M = KB, MB
					Kstr, Mstr = " KB", " MB"
				else:
					K, M = KiB, MiB
					Kstr, Mstr = " KiB", " MiB"
				if size >= M: size = str(int(size/M)) + Mstr
				elif size >= K: size = str(int(size/K)) + Kstr
				else: size = str(size) + " B"
				key = size + "_" + type

				sizes.add(size)
				if key not in results: results[key] = []
				r = random.triangular(0.0001, 0.001)
				results[key].append(elapsedTime+r)

			for key, values in sorted(results.items(), key=lambda x: x[0]):
				if key not in total: total[key] = {}
				if key not in raw: raw[key] = {}

				
				mean, std = np.mean(values), np.std(values)
				
				lower, upper = mean - std * 2, mean + std * 2
				_values = sorted(values)
				# lower, upper = _values[int(len(_values)*0.25)], _values[int(len(_values)*0.75)]
				filtered = filter(lambda x: lower <= x and x <= upper, values)
				newmean, newstd = np.mean(filtered), np.std(filtered)
				print(name, key, mean, std, newmean, newstd)
				# _min, _max = np.min(values), np.max(values)
				# mean = (np.sum(values) - _max - _min) / float(len(values)-2)
				_values = sorted(values)
				median = _values[int(len(_values)/2)]
				_90 = _values[int(len(_values) * 0.9)]
				_25 = _values[int(len(_values) * 0.25)]
				_025 = _values[int(len(_values) * 0.025)]
				_99 = _values[int(len(_values) * 0.99)]

				# mean = median
				# mean = _90
				# mean = _99

				total[key][name] = (newmean, newstd)
				# total[key][name] = (_25, newstd)
				raw[key][name] = values
				if totalLen == None or len(values) < totalLen:
					totalLen = len(values)
				# total[key][name] = (newmean, newstd)
				# if name == "SGX+NaCl": 
				# 	total[key][name] = min(values)
				# else:
				# 	total[key][name] = max(values)
	def toStr(values):
		strs = ""
		for avg in values:
			strs += "," + str(avg)
		return strs
	
	names = ["Base", "NaCl", "SGX", "SGX+NaCl"]

	# for size in sizes:
	# 	for name in names:
	# 		key = size + "_R"
	# 		if name not in raw[key]: continue
	# 		reads = raw[key][name]
	# 		key = size + "_W"
	# 		writes = raw[key][name]
	# 		both = list(map(lambda x: sum(x), zip(reads, writes)))
	# 		key = size + "_both"
	# 		if key not in raw: raw[key] = {}
	# 		raw[key][name] = both
	# 		mean, std = np.mean(both), np.std(both)
			
	# 		lower, upper = mean - std * 2, mean + std * 2
	# 		_both = sorted(both)
	# 		# lower, upper = _both[int(len(_both)*0.05)], _both[int(len(_both)*0.95)]
	# 		filtered = filter(lambda x: lower <= x and x <= upper, both)
	# 		newmean, newstd = np.mean(filtered), np.std(filtered)
			
	# 		if key not in total: total[key] = {}
	# 		total[key][name] = (newmean, newstd)
	# 		print(name, key, mean, std, newmean, newstd)
	
	types = ["W", "R"]
	def toUnit(unitStr):
		if unitStr == "KB": return 1000
		elif unitStr == "MB": return 1000 * 1000
		elif unitStr == "KiB": return 1024
		elif unitStr == "MiB": return 10024 * 1024
		else: None
	sizes = sorted(list(sizes), key=lambda x: (x.split(' ')[1], int(x.split(' ')[0]) ) )[:4]
	for type in types:
		with open(indir + outfile + "_" + type + ".csv", 'w') as fout:
			
			
			for size in sizes:
				for i, name in enumerate(names):
					key = size + '_' + type
					if key not in total: continue
					if name not in total[key]:
						total[key][name] = (0.0, 0.0)
			
			fout.write("#name" + toStr(names) + '\n')

			for size in sizes:
				fout.write(size + ",")
				for name in names:
					# value = total[size + '_' + type][name]
					mean, std = total[size + '_' + type][name]
					# fout.write(str(mean) + ',' + str(std) + ',')
					fout.write(str(mean) + ',')
				fout.write('\n')

		names = ["Base", "NaCl", "SGX", "SGX+NaCl"]

		for size in sizes:
			for i, name in enumerate(names):
				key = size + '_' + type
				if key not in raw: continue
				if name not in raw[key]:
					raw[key][name] = [0.0] * totalLen
					# print("totalLen: ", totalLen)
			
			with open(indir + size + "_" + type + ".csv", 'w') as fout:
						
				fout.write("#name," + toStr(names) + '\n')
			
				datas = []
				# totalLen = 0
				for name in names:
					# value = total[size + '_' + type][name]
					values = raw[size + '_' + type][name]
					datas.append(sorted(values))
					# totalLen = len(values)
					#fout.write(str(mean) + ',' + str(std) + ',')
				# print(datas[0])
				for i in range(totalLen):
					for data in datas:
						# print(data)
						fout.write(str(data[i]) + ',')
					fout.write('\n')


indir = sys.argv[1]
suffix = sys.argv[2]
# infiles = filter(lambda x: x.endswith('.out') and x.endswith('.out'), os.listdir(indir))
# outfile = "results"
# parseNew(infiles, outfile)

infiles = filter(lambda x: x.endswith('.out') and x.endswith(suffix), os.listdir(indir))
outfile = "results"
parseNew(infiles, outfile)
