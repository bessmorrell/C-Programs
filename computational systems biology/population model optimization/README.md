This program finds the exponantial model and logistic model for a population data matrix, and returns which model better fits the data by summing and comparing the absolute value of the distance between the data and each model.

Example output:

>>sum(abs(Data-Exponential)) =
   5.8955e+05

>> sum(abs(Data-Logistic)) =
   2.1739e+05

>>Logistic model is better fit
