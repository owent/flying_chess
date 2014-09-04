using System;
using System.Collections.Generic;
using System.Linq;
using OverCraft.Data.XML;

namespace OverCraft.Data {
	public class Game {

		protected Dice _dice;

		protected List<Player> _players;
		protected List<Plane> _planes;
		protected List<Grid> _grids;

		protected int _nowPlayerIndex;

		public Game() {
			_dice = new Dice();
			_players = new List<Player>();
			_planes = new List<Plane>();
			_grids = new List<Grid>();
			_nowPlayerIndex = 0;
		}

		public int PlayerNumber {
			get {
				return _players.Count;
			}
		}

		public int GridNumber {
			get {
				return _grids.Count;
			}
		}

		public bool IsComplete {
			get {
				return _players.Count(player => !player.IsComplete) < 2;
			}
		}

		public Player GetPlayerByIndex(int index) {
			return index >= 0 && index < _players.Count ? _players[index] : null;
		}

		public Grid GetGridByIndex(int index) {
			return index >= 0 && index < _grids.Count ? _grids[index] : null;
		}

		protected void clear() {
			_players.Clear();
			_planes.Clear();
			_grids.Clear();
			_nowPlayerIndex = 0;
		}

		public Player NextPlayer() {
			if (IsComplete)
				return null;
			while (_players[_nowPlayerIndex = ((_nowPlayerIndex + 1) % _players.Count)].IsComplete)
				;
			return (_players.Count == 0) ? null :
				_players[_nowPlayerIndex];
		}

		protected void addGridToListAndMap(Grid grid, XMLNodeEx node, Dictionary<string, Grid> dict) {
			_grids.Add(grid);
			dict.Add(node.Attributes["ID"], grid);
			grid.PositionX = Convert.ToInt32(node.Attributes["X"]) - 1;
			grid.PositionY = Convert.ToInt32(node.Attributes["Y"]) - 1;
		}

		protected bool addGrids(XMLNodeEx node, Dictionary<string, Grid> dict) {
			if (node.Name == "Grid") {
				addGridToListAndMap(
					(Grid)Activator.CreateInstance(
						Type.GetType("OverCraft.Data." + node.Attributes["Class"]),
						Convert.ToInt32(node.Attributes["Color"])
					), node, dict
				);
				return true;
			}
			return false;
		}

		protected delegate void setGridProperty(Grid startGrid, Grid targetGrid);

		protected Dictionary<string, setGridProperty> initMapping() {
			var _functionMapping = new Dictionary<string, setGridProperty>();
			_functionMapping.Add("Prev", setPrevGird);
			_functionMapping.Add("Next", setNextGird);
			_functionMapping.Add("Jump", setJumpGird);
			_functionMapping.Add("Straight", setStraightGird);
			_functionMapping.Add("Cross", setCrossGird);
			return _functionMapping;
		}

		protected void setPrevGird(Grid startGrid, Grid targetGrid) {
			startGrid.SetPrevGrid = targetGrid;
		}

		protected void setNextGird(Grid startGrid, Grid targetGrid) {
			startGrid.SetNextGrid = targetGrid;
		}

		protected void setJumpGird(Grid startGrid, Grid targetGrid) {
			startGrid.SetJumpGrid = targetGrid;
		}

		protected void setStraightGird(Grid startGrid, Grid targetGrid) {
			((AlterGrid)startGrid).SetStraightGrid = targetGrid;
		}

		protected void setCrossGird(Grid startGrid, Grid targetGrid) {
			((FlyingGrid)startGrid).SetCrossGrid = targetGrid;
		}

		protected bool generateConnection(XMLNodeEx node, Dictionary<string, Grid> dict, Dictionary<string, setGridProperty> functionMapping) {
			var grid = dict[node.Attributes["ID"]];
			node.ChildNodes.All(tmpNode => {
				functionMapping[tmpNode.Name](grid, dict[tmpNode.Attributes["ID"]]);
				return true;
			});
			return true;
		}

		protected Dictionary<string, Grid> generateMapGrids(XMLNodeListEx grids) {
			var dict = new Dictionary<string, Grid>();
			grids.All(node => addGrids(node, dict));
			var functionMapping = initMapping();
			grids.All(node => generateConnection(node, dict, functionMapping));
			return dict;
		}

		protected Plane generatePlane(XMLNodeEx plane, int Color, Dictionary<string, Grid> dict) {
			var ret = new Plane(Color, dict[plane.Attributes["Grid"]]);
			return ret;
		}

		protected Player generatePlaneByPlayer(XMLNodeListEx players, int Color, Dictionary<string, Grid> dict) {
			var planes = new List<Plane>();
			players.All(node => {
				planes.Add(generatePlane(node, Color, dict));
				return true;
			});
			return new Player(planes, planes[0].Color);
		}

		protected void generatePlayers(XMLNodeListEx players, Dictionary<string, Grid> dict) {
			players.All(node => {
				_players.Add(generatePlaneByPlayer(node.ChildNodes, Convert.ToInt32(node.Attributes["Color"]), dict));
				return true;
			});
		}

		public void GenerateFromXMLFile(string xml) {
			clear();
			var document = new XMLDocumentEx();
			document.Load(xml);
			generatePlayers(document.GetFirstChild("Players").ChildNodes,
				generateMapGrids(document.GetFirstChild("Grids").ChildNodes));
		}

		public int ThrowDice {
			get {
				return _dice.Throw();
			}
		}
	}
}
