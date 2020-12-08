import pandas as pd
from rake_nltk import Rake
import numpy as np
from sklearn.metrics.pairwise import cosine_similarity
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.feature_extraction.text import TfidfVectorizer

df = pd.read_csv('test_data.csv')
df = df[['Title','Genre','Director','Actors','Plot']]
# df.head()

df['Actors'] = df['Actors'].map(lambda x: x.split(',')[:3])
df['Genre'] = df['Genre'].map(lambda x: x.lower().split(','))
df['Director'] = df['Director'].map(lambda x: x.split(' '))

# merging together first and last name for each actor and director, so it's considered as one word 
# and there is no mix up between people sharing a first name
for index, row in df.iterrows():
	row['Actors'] = [x.lower().replace(' ','') for x in row['Actors']]
	row['Director'] = ''.join(row['Director']).lower()

# initializing the new column
df['Key_words'] = ""

for index, row in df.iterrows():
	plot = row['Plot']
				    
	# instantiating Rake, by default it uses english stopwords from NLTK
	# and discards all puntuation characters as well
	r = Rake()

	# extracting the words by passing the text
	r.extract_keywords_from_text(plot)

	# getting the dictionary whith key words as keys and their scores as values
	key_words_dict_scores = r.get_word_degrees()
	    
	# assigning the key words to the new column for the corresponding movie
	row['Key_words'] = list(key_words_dict_scores.keys())

# dropping the Plot column
df.drop(columns = ['Plot'], inplace = True)

df.set_index('Title', inplace = True)

df['bag_of_words'] = ''
columns = df.columns
for index, row in df.iterrows():
	words = ''
	for col in columns:
		if col != 'Director':
			words = words + ' '.join(row[col])+ ' '
		else:
			words = words + row[col]+ ' '
	row['bag_of_words'] = words
				     
df.drop(columns = [col for col in df.columns if col!= 'bag_of_words'], inplace = True)

# instantiating and generating the count matrix
count = TfidfVectorizer() #CountVectorizer()
count_matrix = count.fit_transform(df['bag_of_words'])

# generating the cosine similarity matrix
cosine_sim = cosine_similarity(count_matrix, count_matrix)

f = open("data/cosine_similarity.txt", 'w')

for i, sublist in enumerate(cosine_sim):
	for j in range(len(sublist)):
		f.write(str(round(sublist[j]*100, 3)) + " ")
	
	f.write("\n")

f.close()


# creating a Series for the movie titles so they are associated to an ordered numerical
# list I will use in the function to match the indexes
indices = pd.Series(df.index)
indices[:5]

#  defining the function that takes in movie title 
# as input and returns the top 10 recommended movies
def recommendations(idx, cosine_sim = cosine_sim):
		    
	# initializing the empty list of recommended movies
	recommended_movies = []
				    
	# gettin the index of the movie that matches the title
	# idx = indices[indices == title].index[0]

	# creating a Series with the similarity scores in descending order
	score_series = pd.Series(cosine_sim[idx]).sort_values(ascending = False)

	# getting the indexes of the 10 most similar movies
	top_10_indexes = list(score_series.iloc[1:11].index)
	    
	# populating the list with the titles of the best 10 matching movies
	for i in top_10_indexes:
		recommended_movies.append(list(df.index)[i])
	        
	print (recommended_movies)
	return recommended_movies


recommendations(3)

