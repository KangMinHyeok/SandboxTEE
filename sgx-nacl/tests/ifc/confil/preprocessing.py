import pandas as pd
from rake_nltk import Rake
import numpy as np
from sklearn.metrics.pairwise import cosine_similarity
from sklearn.feature_extraction.text import CountVectorizer

df = pd.read_csv('test_data.csv')

df = df[['Title','Genre','Director','Actors','Plot']]
df.head()


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



