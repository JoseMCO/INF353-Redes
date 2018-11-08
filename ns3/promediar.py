import pandas as pd
import matplotlib.pyplot as plt

names = ["Round","nodes alive","sent","avg energy","max energy","min energy"]
minimum = [999999999999999, 0]

columnas_nodes_alive = pd.DataFrame() 
columnas_avg_energy = pd.DataFrame() 
columnas_max_energy = pd.DataFrame()  
columnas_min_energy = pd.DataFrame()

resumen = pd.DataFrame()
resumen_leach = pd.DataFrame()  
resumen_sep = pd.DataFrame()  
resumen_teen = pd.DataFrame()  

for x in ["leach", "sep", "teen"]:
	for i in range(0,5):
		file = pd.read_csv("/home/ente/Desktop/INF353-Redes/ns3/output/"+ x + "_simulation_" + str(i) + ".csv")
		if(file.shape[0] < minimum[0]):
			minimum[0] = file.shape[0]
			minimum[1] = file[names[0]]

		columnas_nodes_alive[names[1] + str(i)] = file[names[1]]
		columnas_avg_energy[names[3] + str(i)] = file[names[3]]
		columnas_max_energy[names[4] + str(i)] = file[names[4]]
		columnas_min_energy[names[5] + str(i)] = file[names[5]]
		
	resta = columnas_max_energy.shape[0] - minimum[0]

	resumen['Round'] = minimum[1]

	columnas_nodes_alive.drop(columnas_nodes_alive.tail(resta).index , inplace=True)
	columnas_max_energy.drop(columnas_max_energy.tail(resta).index , inplace=True)
	columnas_min_energy.drop(columnas_min_energy.tail(resta).index , inplace=True)
	columnas_avg_energy.drop(columnas_avg_energy.tail(resta).index , inplace=True)

	resumen['avg_nodes_alive'] = columnas_nodes_alive.mean(axis=1)
	resumen['avg_max_energy'] = columnas_max_energy.mean(axis=1)
	resumen['avg_min_energy'] = columnas_min_energy.mean(axis=1)
	resumen['avg_avg_energy'] = columnas_avg_energy.mean(axis=1)
	
	if(x == "leach"):
		resumen_leach['Round'] = minimum[1]
		resumen_leach['avg_nodes_alive'] = columnas_nodes_alive.mean(axis=1)
		resumen_leach['avg_max_energy'] = columnas_max_energy.mean(axis=1)
		resumen_leach['avg_min_energy'] = columnas_min_energy.mean(axis=1)
		resumen_leach['avg_avg_energy'] = columnas_avg_energy.mean(axis=1)

	elif(x == "sep"):
		resumen_sep['Round'] = minimum[1]
		resumen_sep['avg_nodes_alive'] = columnas_nodes_alive.mean(axis=1)
		resumen_sep['avg_max_energy'] = columnas_max_energy.mean(axis=1)
		resumen_sep['avg_min_energy'] = columnas_min_energy.mean(axis=1)
		resumen_sep['avg_avg_energy'] = columnas_avg_energy.mean(axis=1)

	else:	
		resumen_teen['Round'] = minimum[1]	
		resumen_teen['avg_nodes_alive'] = columnas_nodes_alive.mean(axis=1)
		resumen_teen['avg_max_energy'] = columnas_max_energy.mean(axis=1)
		resumen_teen['avg_min_energy'] = columnas_min_energy.mean(axis=1)
		resumen_teen['avg_avg_energy'] = columnas_avg_energy.mean(axis=1)

	resumen.to_csv("avg_" + x + "_simulation.csv", sep=',', index=False)

	ax = plt.gca()
	resumen.plot(kind='line', x='Round', y='avg_max_energy', ax=ax)
	resumen.plot(kind='line', x='Round', y='avg_min_energy', ax=ax)
	resumen.plot(kind='line', x='Round', y='avg_avg_energy', ax=ax)

	plt.legend(('Max Energy', 'Min Energy', 'AVG Energy'), loc='upper right')
	plt.savefig( x + "_simulation.png")
	plt.gcf().clear()

	minimum = [999999999999999, 0]
	columnas_nodes_alive = pd.DataFrame() 
	columnas_avg_energy = pd.DataFrame() 
	columnas_max_energy = pd.DataFrame()  
	columnas_min_energy = pd.DataFrame()
	resumen = pd.DataFrame()  

ax = plt.gca()

resumen_leach.plot(kind='line', x='Round', y='avg_avg_energy', ax=ax)
resumen_sep.plot(kind='line', x='Round', y='avg_avg_energy', ax=ax)
resumen_teen.plot(kind='line', x='Round', y='avg_avg_energy', ax=ax)
plt.legend(('LEACH', 'SEP', 'TEEN'), loc='upper right')
plt.savefig( "avg_energy_all_models.png")
plt.gcf().clear()

ax = plt.gca()

resumen_leach.plot(kind='line', x='Round', y='avg_min_energy', ax=ax)
resumen_sep.plot(kind='line', x='Round', y='avg_min_energy', ax=ax)
resumen_teen.plot(kind='line', x='Round', y='avg_min_energy', ax=ax)
plt.legend(('LEACH', 'SEP', 'TEEN'), loc='upper right')
plt.savefig( "min_energy_all_models.png")
plt.gcf().clear()

ax = plt.gca()

resumen_leach.plot(kind='line', x='Round', y='avg_max_energy', ax=ax)
resumen_sep.plot(kind='line', x='Round', y='avg_max_energy', ax=ax)
resumen_teen.plot(kind='line', x='Round', y='avg_max_energy', ax=ax)
plt.legend(('LEACH', 'SEP', 'TEEN'), loc='upper right')
plt.savefig( "max_energy_all_models.png")
plt.gcf().clear()

ax = plt.gca()

resumen_leach.plot(kind='line', x='Round', y='avg_nodes_alive', ax=ax)
resumen_sep.plot(kind='line', x='Round', y='avg_nodes_alive', ax=ax)
resumen_teen.plot(kind='line', x='Round', y='avg_nodes_alive', ax=ax)
plt.legend(('LEACH', 'SEP', 'TEEN'), loc='upper right')
plt.savefig( "avg_nodes_alive_all_models.png")
plt.gcf().clear()

